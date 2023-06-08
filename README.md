# interview2
json parsing into tlv

Usage: reader--input json_to_process --output output_tlv_file --dic output_dictionary_file

Example: /reader --input example.json --output example.tlv --dic dictionary.tlv

a sample example.json file is included.

simple tests present in /tests/ folder

the expected input file format:

{"key1": "value1", "key2": 42, "key3": true}
{"key1": "value1", "key2": 42, "key3": true}

supported types: string, int, boolean

test read from tlv file by calling ./reader --test
