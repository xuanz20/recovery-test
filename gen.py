import random
import string

NUM = 2000000

def generate_name(existing_names):
    while True:
        name_length = 64
        name = ''.join(random.choices(string.ascii_letters + string.digits, k=name_length))
        if name not in existing_names:
            return name

def generate_isbn():
    isbn = ''.join(random.choices(string.digits, k=192))
    return isbn

book_names = set()
book_pairs = []
while len(book_pairs) < NUM:
    name = generate_name(book_names)
    isbn = generate_isbn()
    book_pairs.append((name, isbn))
    book_names.add(name)

with open("data.sql", "w") as file:
    for name, isbn in book_pairs:
        file.write(f'INSERT INTO booklist VALUES ("{name}", "{isbn}");\n')

with open("init.txt", "w") as file:
    for name, isbn in book_pairs:
        file.write(f'{name},{isbn}\n')