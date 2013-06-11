export LD_LIBRARY_PATH=../../lib/c/

java -cp .:../../lib/java:gen Encoder encoded_data
./example_decoder encoded_data
./example_encoder encoded_data
java -cp .:../../lib/java:gen Decoder encoded_data

echo "running python decoder (from wiki_example):"
PYTHONPATH=../../lib/python ../wiki_example/example_decoder.py encoded_data

