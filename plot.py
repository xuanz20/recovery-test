import matplotlib.pyplot as plt

with open("throughput_5.txt") as f:
    result = [float(line.strip()) for line in f]

plt.figure(1)
plt.plot(result)
plt.title("50%read, 50%update")
plt.xlabel("seconds")
plt.ylabel("throughput")
plt.savefig("50%read_50%update.png")

with open("throughput_9.txt") as f:
    result = [float(line.strip()) for line in f]

plt.figure(2)
plt.plot(result)
plt.title("90%read, 10%update")
plt.xlabel("seconds")
plt.ylabel("throughput")
plt.savefig("90%read_10%update.png")
