export LD_LIBRARY_PATH=../../lib/c/
echo
echo "********************************************"
echo "***   Running example for version 2013   ***"
echo "********************************************"
echo

java -cp .:../../lib/java:gen Encoder encoded_data
./example_decoder encoded_data
./example_encoder encoded_data
java -cp .:../../lib/java:gen Decoder encoded_data

echo "running python decoder (from wiki_example):"
PYTHONPATH=../../lib/python ../wiki_example/example_decoder.py encoded_data LabComm2013

echo
echo "*******************************************************"
echo "***   Running C and Java example for version 2006   ***"
echo "*******************************************************"
echo

java -cp .:../../lib/java:gen06 Encoder06 encoded_data06
./example_decoder06 encoded_data06
./example_encoder06 encoded_data06
java -cp .:../../lib/java:gen06 Decoder06 encoded_data06

echo "running python decoder (from wiki_example):"
PYTHONPATH=../../lib/python ../wiki_example/example_decoder.py encoded_data06 LabComm2006
