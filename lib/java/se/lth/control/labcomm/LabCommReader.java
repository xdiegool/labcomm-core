package se.lth.control.labcomm;

public interface LabCommReader {

  public void handle(byte[] data, int begin, int end);

}
