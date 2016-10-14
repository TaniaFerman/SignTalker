import tensorflow as tf

filename_queue = tf.train.string_input_producer(
    tf.train.match_filenames_once("./csvs/*.txt"))

reader = tf.TextLineReader()
key, value = reader.read(filename_queue)

record_defaults = [[1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
                   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
				   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
				   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
				   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
				   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
				   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],
				   [1,1], [1,1], [1,1], [1,1], [1,1],[1,1],[1,1],[1,1],["a"]]
col1, col2, col3, col4, col5, col6, col7, col8 = tf.decode_csv(
    value, record_defaults=record_defaults)
features = tf.pack([col1, col2, col3, col4])