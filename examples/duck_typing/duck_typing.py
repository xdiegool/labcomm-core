#!/usr/bin/python

import labcomm2014
import animal
import StringIO

class Animal:
    def __init__(self):
        self.says = None

if __name__ == '__main__':
    buf = StringIO.StringIO()
    encoder = labcomm2014.Encoder(labcomm2014.StreamWriter(buf))
    encoder.add_decl(animal.cow.signature)
    encoder.add_decl(animal.dog.signature)
    encoder.add_decl(animal.duck.signature)
    theAnimal = Animal()
    theAnimal.says = 'Moo'
    encoder.encode(theAnimal, animal.cow.signature)
    theAnimal.says = 'Bow-Wow'
    encoder.encode(theAnimal, animal.dog.signature)
    theAnimal.says = 'Quack'
    encoder.encode(theAnimal, animal.duck.signature)
    buf.seek(0)
    decoder = labcomm2014.Decoder(labcomm2014.StreamReader(buf))
    try:
        while True:
            value,decl = decoder.decode()
            if value:
                print decl.name, 'says', value
                pass
            pass
        pass
    except EOFError:
        pass
    pass
