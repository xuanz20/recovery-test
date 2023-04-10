import matplotlib.pyplot as plt

with open("throughput_5_noRDB.txt") as f:
    result = [float(line.strip()) for line in f]
plt.figure(1)
plt.plot(result)
plt.title("50%read, 50%update, RDB off")
plt.xlabel("seconds")
plt.ylabel("throughput")
plt.savefig("result/50%read_50%update_noRDB.png")

with open("throughput_9_noRDB.txt") as f:
    result = [float(line.strip()) for line in f]
plt.figure(2)
plt.plot(result)
plt.title("90%read, 10%update, RDB off")
plt.xlabel("seconds")
plt.ylabel("throughput")
plt.savefig("result/90%read_10%update_noRDB.png")
