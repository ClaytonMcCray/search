from os import system, path, makedirs, getcwd, chdir, sep, walk
from time import time
from random import randrange, randint
from shutil import rmtree
from datetime import datetime

class conf:
	alphabet = list("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789")
	searchable_file_name = "test.log"
	test_dir = "test_dir"
	dir_csv = "comparison_dir.csv"


def generate_search_dir(depth=10, width=3, num_files_in_dir=1):
	if path.exists(conf.test_dir):
		rmtree(conf.test_dir)
	
	makedirs(conf.test_dir)
	
	working_directory = getcwd()
	chdir(conf.test_dir)

	permutable_dir_set = [[i for i in range(width)] for k in range(depth)]

	for _ in range(50):
		cur = ""
		for level in permutable_dir_set:
			cur += str(level[randrange(min(level), max(level) + 1)]) + sep
		makedirs(cur)

	chdir(working_directory)

	f = generate_searchable_file(write_out=False)
	for dirpath, _, __ in walk(conf.test_dir):
		for s in range(num_files_in_dir):
			with open(dirpath + sep + conf.searchable_file_name + str(s), "w", encoding="utf-8") as _f:
				_f.writelines(f)


def generate_searchable_file(line_count=1000, write_out=True):
	file_text = []
	for _ in range(line_count):
		line = ""
		for __ in range(randrange(1, 10000)):  # line length
			line += conf.alphabet[randrange(len(conf.alphabet))]

		file_text.append(line)

	if (len(file_text) != line_count):
		print("ERROR")
		exit(1)

	if write_out:
		with open(conf.searchable_file_name, "w", encoding="utf-8") as f:
			f.writelines(file_text)
	else:
		return file_text


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


def search_dir_with_search(query):
	start = time()
	system("../release/search " + path.abspath(conf.test_dir) + " \"" + query + "\"")
	end = time()
	return end - start


def search_file_with_grep(query):
	start = time()
	system("grep -InH" + " \"" + query + "\" " + path.abspath(conf.searchable_file_name))
	end = time()
	return end - start


def search_dir_with_grep(query):
	start = time()
	system("grep -InHr" + " \"" + query + "\" " + path.abspath(conf.test_dir))
	end = time()
	return end - start


def search_file_with_ripgrep(query):
	start = time()
	system("rg " + " \"" + query + "\" " + path.abspath(conf.searchable_file_name))
	end = time()
	return end - start


def search_dir_with_ripgrep(query):
	start = time()
	system("rg " + " \"" + query + "\" " + path.abspath(conf.test_dir))
	end = time()
	return end - start




def avg_time(num_tries, max_query_len, search_, grep_, ripgrep_, csv=None):
	qs = [generate_search_query(randrange(2, max_query_len)) for i in range(num_tries)]

	searches = []
	greps = []
	ripgreps = []
	for q in qs:
		searches.append(search_(q))
		greps.append(grep_(q))
		ripgreps.append(ripgrep_(q))

		if csv is not None:
			with open(csv, "a", encoding="utf-8") as csv_f:
				lines = datetime.today().strftime('%Y-%m-%d') + "," + q + "," + str(len(q)) \
					+ "," + str(searches[-1]) + "," + str(greps[-1]) + "," + str(ripgreps[-1]) + "\n"

				csv_f.write(lines)


	return {"search": sum(searches)/len(searches),
		"grep": sum(greps)/len(greps),
		"ripgrep": sum(ripgreps)/len(ripgreps)}


# generate_searchable_file()
# print("done generating file")
# print(avg_file_time(10))

if __name__ == "__main__":
	generate_search_dir()
	print("done generating files/tree")
	print(avg_time(97, 10000, search_dir_with_search, search_dir_with_grep, search_dir_with_ripgrep, csv=conf.dir_csv))
