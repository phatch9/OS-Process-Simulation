import multiprocessing

def input_process(pipe_out):
    while True:
        user_input = input("$ ")
        pipe_out.send(user_input)
        if user_input == "T":
            break

def output_process(pipe_in):
    while True:
        received_char = pipe_in.recv()
        print(f"Received: {received_char}")
        if received_char == "T":
            break

if __name__ == "__main__":
    # Create a pipe
    pipe_in, pipe_out = multiprocessing.Pipe()

    # Create processes
    p1 = multiprocessing.Process(target=input_process, args=(pipe_out,))
    p2 = multiprocessing.Process(target=output_process, args=(pipe_in,))

    # Start processes
    p1.start()
    p2.start()

    # Wait for processes to complete
    p1.join()
    p2.join()
