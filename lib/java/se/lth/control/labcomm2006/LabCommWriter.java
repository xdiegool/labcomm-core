package se.lth.control.labcomm2006;

import java.io.IOException;

public interface LabCommWriter {

  public void write(byte[] data) throws IOException;

}
