import time
import random

def main():
    counter = 0
    print("Print data (Press Ctrl+C to stop) ...")
    try:
        while True:
            counter += 1
            current_time = time.strftime("%Y-%m-%d %H:%M:%S")
            random_num = random.randint(1, 10000)

            print(f"It is [{current_time}] Count: {counter}, Radom: {random_num}", flush=True)
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nStopped")

if __name__ == "__main__":
    main()