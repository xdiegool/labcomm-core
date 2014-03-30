using System;

namespace se.lth.control.labcomm {

  public interface LabCommDecoder {

    void register(LabCommDispatcher dispatcher, 
		  LabCommHandler handler);

    bool decodeBoolean();
    byte decodeByte();
    short decodeShort();
    int decodeInt();
    long decodeLong();
    float decodeFloat();
    double decodeDouble();
    String decodeString();
    int decodePacked32();

  }

} 
