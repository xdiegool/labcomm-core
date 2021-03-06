export LD_LIBRARY_PATH=../../lib/c/
echo
echo "********************************************"
echo "***   Running example for version 2013   ***"
echo "********************************************"
echo

java -cp .:../../lib/java/labcomm2014.jar:gen Encoder encoded_data
./example_decoder encoded_data

PYTHONPATH=../../lib/python:gen ./example_encoder.py encoded_data
java -cp .:../../lib/java/labcomm2014.jar:gen Decoder encoded_data

./example_encoder encoded_data
java -cp .:../../lib/java/labcomm2014.jar:gen Decoder encoded_data

echo "running python decoder (from wiki_example):"
PYTHONPATH=../../lib/python ../wiki_example/example_decoder.py encoded_data LabComm2014

