export LD_LIBRARY_PATH=../../lib/c/
echo
echo "********************************************"
echo "***   Running example for version 2013   ***"
echo "********************************************"
echo

java -cp .:../../lib/java/labcomm2014.jar:gen Encoder encoded_data

echo "running Java  decoder:"
java -cp .:../../lib/java/labcomm2014.jar:gen Decoder encoded_data

echo "running C decoder:"
./example_decoder encoded_data

echo "running python decoder (from wiki_example):"
PYTHONPATH=../../lib/python ../wiki_example/example_decoder.py encoded_data LabComm2014

echo "running C encoder:"
./example_encoder encoded_data

echo "running Java  decoder:"
java -cp .:../../lib/java/labcomm2014.jar:gen Decoder encoded_data

echo "running C decoder:"
./example_decoder encoded_data

echo "running python decoder (from wiki_example):"
PYTHONPATH=../../lib/python ../wiki_example/example_decoder.py encoded_data LabComm2014

echo "running python encoder:"
PYTHONPATH=../../lib/python:gen ./example_encoder.py encoded_data2

echo "running Java  decoder:"
java -cp .:../../lib/java/labcomm2014.jar:gen Decoder encoded_data2

echo "running C decoder:"
./example_decoder encoded_data2

