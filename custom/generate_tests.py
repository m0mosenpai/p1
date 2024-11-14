import string
import random


def getBoard(astr):
    # create 200 possible boards
    possible_boards = []
    for i in range(200):
        # pick a random number of characters
        board = []
        i = 0
        # choose a random split point
        split_point = random.randrange(1, len(astr))
        while (i < len(astr)):
            split_point = random.randrange(0, len(astr))
            side = astr[i:i + split_point]
            if side == "":
                continue

            board.append(side)
            i += split_point

        if board not in possible_boards:
            possible_boards.append(board)

    return possible_boards


with open("../dict.txt", "r") as f:
    words = f.readlines()

word_dict = {}
for i, w in enumerate(words):
    if not word_dict.get(w[0], None):
        word_dict[w[0]] = [w[:-1]]
    else:
        word_dict[w[0]].append(w[:-1])

inputs, boards = [], []
alphabets = list(string.ascii_lowercase)
for il in range(1, 5):
    i = 0
    curr_a = alphabets[random.randrange(len(alphabets))]
    used = set()
    sols = []
    while i < il:
        random_w = word_dict[curr_a][random.randrange(len(word_dict[curr_a]))]
        if random_w not in used:
            used.add(random_w)
            sols.append(random_w)
            curr_a = random_w[-1]
            i += 1

    inputs.append(sols)

    uniques = ''.join(set(''.join(sols)))
    possible_boards = getBoard(uniques)
    boards.append(possible_boards)

for i in range(len(inputs)):
    print("{} ->\n{}".format(inputs[i], boards[i]))
    print("")


# alphabets = list(string.ascii_lowercase)
# count = 1
# while count <= 5:
#     # shuffle all lowercase alphabets
#     random.shuffle(alphabets)
#     # create a string
#     astr = ''.join(alphabets)
#     # pick a random number of characters
#     chars = random.randrange(3, len(astr))
#     astr = astr[:chars]
#     # choose a random split point
#     boards = []
#
#     i = 0
#     split_point = random.randrange(1, len(astr))
#     while (i < len(astr)):
#         split_point = random.randrange(0, len(astr))
#         side = astr[i:i + split_point]
#         if side == "":
#             continue
#
#         boards.append(side)
#         i += split_point
#
#     board_file = "{}.board".format(count)
#     # create the board file
#     with open("tests/{}".format(board_file), "x") as f:
#         for side in boards:
#             f.write("{}\n".format(side))
#
#     # create the run file
#     run_file = "{}.run".format(count)
#     with open("tests/{}".format(run_file), "x") as f:
#         f.write("../solution/letter-boxed tests/{}.board ../dict.txt < tests/{}.in\n".format(count, count))
#
#     # create the err file
#     err_file = "{}.err".format(count)
#     with open("tests/{}".format(err_file), "x") as f:
#         f.write("")
#
#     count += 1
