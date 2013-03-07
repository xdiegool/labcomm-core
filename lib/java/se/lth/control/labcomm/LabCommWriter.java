package se.lth.control.labcomm;

import java.io.IOException;

public interface LabCommWriter {

  public void write(byte[] data) throws IOException;

}
