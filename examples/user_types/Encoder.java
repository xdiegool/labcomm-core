import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;

import se.lth.control.labcomm.EncoderChannel;

/**
 * Simple encoder 
 */
public class Encoder 
{

  EncoderChannel encoder;

  public Encoder(OutputStream out) 
    throws Exception 
  {
    encoder = new EncoderChannel(out);
    twoInts.register(encoder);
    twoLines.register(encoder);
    theFirstInt.register(encoder);
    theSecondInt.register(encoder);
  }

  public void doEncode() throws java.io.IOException {
    twoInts ti = new twoInts();
    ti.a = 12;
    ti.b = 21;

    System.out.println("Encoding twoInts");
    twoInts.encode(encoder, ti);
      
    System.out.println("Encoding the Ints");
    theFirstInt.encode(encoder, 71);
    theSecondInt.encode(encoder, 24);

    twoLines x = new twoLines();
    line l1 = new line();
    point p11 = new point();
    coord c11x = new coord();
    coord c11y = new coord();
    c11x.val = 11;
    c11y.val = 99;
    p11.x = c11x;
    p11.y = c11y;

    l1.start = p11;
    
    point p12 = new point();
    coord c12x = new coord();
    coord c12y = new coord();
    c12x.val = 22;
    c12y.val = 88;
    p12.x = c12x;
    p12.y = c12y;

    l1.end = p12;

    line l2 = new line();
    point p21 = new point();
    coord c21x = new coord();
    coord c21y = new coord();
    c21x.val = 17;
    c21y.val = 42;
    p21.x = c21x;
    p21.y = c21y;

    l2.start = p21;
    
    point p22 = new point();
    coord c22x = new coord();
    coord c22y = new coord();
    c22x.val = 13;
    c22y.val = 37;
    p22.x = c22x;
    p22.y = c22y;

    l2.end = p22;

    foo f = new foo();
    f.a = 10;
    f.b = 20;
    f.c = false;

    x.l1 = l1;
    x.l2 = l2;
    x.f  = f;

    System.out.println("Encoding theTwoLines");
    twoLines.encode(encoder, x);
  }


  public static void main(String[] arg) throws Exception {
    FileOutputStream fos = new FileOutputStream(arg[0]);
    Encoder example = new Encoder(fos);
    example.doEncode();
    fos.close();
  }

}

