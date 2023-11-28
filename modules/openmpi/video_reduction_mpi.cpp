#include <iostream>
#include <mpi.h>
#include "../common/lib.cpp"
#include "../common/lib_mpi.cpp"


int main(int argc, char* argv[])
{
    std::string input_path;
    std::string output_path;

    if (argc == 1)
    {
        std::cout << "[ERROR] No arguments passed, can't initiate MPI enviroment." << '\n';
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "[INFO] Arguments passed, creating objects and paths..." << '\n';
        input_path = argv[1];
        output_path = argv[2];
        MPI::Init(argc, argv);
    }

    // Alias for the common world
    const MPI::Intracomm& World = MPI::COMM_WORLD;

    const int rank = World.Get_rank();
    const int n_process = World.Get_size();

    auto cap     = invoke_capture(input_path);
    const auto video_props  = VideoProps(cap);
    auto writer             = invoke_writer(output_path, video_props, 3);

    if (rank == 0) std::cout << "[INFO] Starting video compression..." << '\n';

    std::cout << "Hello from process " << rank << "!";

    for (int i = 0; i < video_props.frame_count; i += n_process)
    {
        std::vector<cv::Mat> frames(n_process);
        std::vector<cv::Mat> deserialized_frames(n_process);
        std::vector<cv::Mat> compressed_frames(n_process);
        std::vector<UCMat> serialized_frames(n_process);
        cv::Mat frame;
        cv::Mat compressed_frame;


        if (rank == 0) // Main node
        {
            // Load n_process frames
            for (int loaded_frames = 0; loaded_frames < n_process; loaded_frames++)
            {
                if (cap.read(frame))
                {
                    frames[i] = frame;
                    std::cout << "Loaded " << i << " frames from main node." << std::endl;
                }
                else break;
            }

            // Serialize frames
            for (int sf = 0; sf < n_process; sf++)
            {
                println("Serialized a frame omg");
                serialized_frames[sf] = serialize_frame(frames[sf]);
            }

            // Broadcast frames
            try
            {
                for (auto sf : serialized_frames)
                    World.Bcast(
                        sf.data(),
                        sf.size(),
                        MPI::UNSIGNED_CHAR, 0
                    );
            }
            catch (MPI::Exception &err)
            {
                std::cout << "[ERROR] Error in Bcast: "
                << err.Get_error_string()
                << " with error "
                << err.Get_error_code()
                << '\n';
            }

            World.Barrier();

        }

        else // Other nodes
        {
            UCMat serialized_frame = serialized_frames[rank];
            std::cout << "[DEBUG] Got frame in process " << rank << '\n';

            // Receive broadcasted frames
            try
            {
                World.Bcast(
                    serialized_frame.data(),
                    serialized_frame.size(),
                    MPI::UNSIGNED_CHAR, 0
                );
            }
            catch (MPI::Exception &err)
            {
                std::cout << "[ERROR] Error in Bcast: "
                << err.Get_error_string()
                << " with error "
                << err.Get_error_code()
                << '\n';
            }

            // Deserialize compressed frame
            deserialized_frames[rank] = deserialize_frame(serialized_frames[rank]);

        }

        compressed_frame = compression_algorithm(deserialized_frames[rank], 3);
        
        // Dont know how to gather
        
        
        if (rank == 0)
        {
            for (int j = 0; j < n_process; j++)
            {
                writer.write(compressed_frames[j]);
            }

            double process_time = end_time - start_time;
        
            printf("\nStarting compression using %d processes\n", numProcesses);

        
            FILE *file = fopen("tiempos_mpi_video.txt", "a");
            if (file != NULL) {
                fprintf(file, "Total time with %d process: %fs\n", numProcesses, end_time - start_time);
                fclose(file);
            } else {
                printf("Error opening the file...");
            }
        }
    }


    if (rank == 0) std::cout << "[INFO] Finishing video compression..." << '\n';

    cap.release();
    writer.release();

    MPI::Finalize();

    return EXIT_SUCCESS;
}
