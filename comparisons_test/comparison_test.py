from os import system, path
from time import time
from random import randrange

class conf:
	alphabet = list("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789")
	searchable_file_name = "test.log"

def generate_searchable_file(line_count=1000):
	file_text = []
	for _ in range(line_count):
		line = ""
		for __ in range(randrange(1, 10000)):  # line length
			line += conf.alphabet[randrange(len(conf.alphabet))]

		file_text.append(line)

	if (len(file_text) != line_count):
		print("ERROR")
		exit(1)

	with open(conf.searchable_file_name, "w", encoding="utf-8") as f:
		f.writelines(file_text)


def generate_search_query(q_length=100):
	line = ""
	for _ in range(randrange(1, q_length)):  # line length
		line += conf.alphabet[randrange(len(conf.alphabet))]

	return line


def search_file_with_search(query):
	start = time()
	system("../release/search " + path.abspath(conf.searchable_file_name) + " \"" + query + "\"")
	end = time()
	return end - start


def search_file_with_grep(query):
	start = time()
	system("grep -InH" + " \"" + query + "\" " + path.abspath(conf.searchable_file_name))
	end = time()
	return end - start


def search_file_with_ripgrep(query):
	start = time()
	system("rg " + " \"" + query + "\" " + path.abspath(conf.searchable_file_name))
	end = time()
	return end - start


def avg_file_time(num_tries):
	qs = [generate_search_query(3) for i in range(num_tries)]
	searches = [search_file_with_search(q) for q in qs]
	greps = [search_file_with_grep(q) for q in qs]
	ripgreps = [search_file_with_ripgrep(q) for q in qs]

	return {"search": sum(searches)/len(searches),
		"grep": sum(greps)/len(greps),
		"ripgrep": sum(ripgreps)/len(ripgreps)}


generate_searchable_file()
print("done generating file")
print(avg_file_time(10))
