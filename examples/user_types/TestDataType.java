import java.io.PrintStream;

import se.lth.control.labcomm2014.*;

/**
 * Test data type tree
 */
public class TestDataType
{

  private PrintStream out;
  public TestDataType(PrintStream out)
    throws Exception
  {
      this.out = out;
  }

  public void doTest() throws java.io.IOException {
    twoLines x = new twoLines();
    DataType.printDataType(out, x.getDispatcher());

    twoStructsAndInt y = new twoStructsAndInt();
    DataType.printDataType(out, y.getDispatcher());

    theFirstInt z = new theFirstInt();
    DataType.printDataType(out, z.getDispatcher());

    doavoid a = new doavoid();
    DataType.printDataType(out, a.getDispatcher());

  }

  public static void main(String[] arg) throws Exception {
    TestDataType example = new TestDataType(System.out);
    example.doTest();
  }

}

