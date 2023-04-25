import random
import string

key_len = 8
value_len = 24
NUM = 1000

def generate_key(existing_keys):
    while True:
        key = ''.join(random.choices(string.ascii_letters + string.digits, k=key_len))
        if key not in existing_keys:
            return key

def generate_value():
    value = ''.join(random.choices(string.digits, k=value_len))
    return value

key_set = set()
kv_pair = []
while len(kv_pair) < NUM:
    key = generate_key(key_set)
    value = generate_value()
    kv_pair.append((key, value))
    key_set.add(key)

count = 0
with open("data.sql", "w") as file:
    for key, value in kv_pair:
        if count == 0:
            file.write("INSERT INTO test_table VALUES")
        count += 1
        file.write(f'("{key}", "{value}")')
        if count == 1000:
            file.write(";\n")
            count = 0
        else:
            file.write(",")

with open("init.txt", "w") as file:
    for key, value in kv_pair:
        file.write(f'{key},{value}\n')
