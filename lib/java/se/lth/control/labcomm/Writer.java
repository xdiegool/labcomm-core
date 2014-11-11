package se.lth.control.labcomm;

import java.io.IOException;

public interface Writer {

  public void write(byte[] data) throws IOException;

}
