using System;

namespace se.lth.control.labcomm2014 {

  public interface Decoder : AbstractDecoder {

    bool decodeBoolean();
    byte decodeByte();
    short decodeShort();
    int decodeInt();
    long decodeLong();
    float decodeFloat();
    double decodeDouble();
    String decodeString();
    int decodePacked32();
    SampleDispatcher decodeSampleRef();

  }

} 
