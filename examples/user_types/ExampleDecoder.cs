using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using se.lth.control.labcomm;

namespace user_types
{
    class Decoder : twoLines.Handler 
    {
        DecoderChannel dec;

        public Decoder(Stream stream)
        {
            dec = new DecoderChannel(stream);
            twoLines.register(dec, this);
            try
            {
                Console.WriteLine("Running decoder.");
                dec.run();
            }
            catch (EndOfStreamException)
            {
                Console.WriteLine("EOF reached");
            }
        }

        private string genPoint(point p)
        {
            return "(" + p.x.val + ", " + p.y.val + ")";
        }
        private String genLine(line l)
        {
            return "Line from " + genPoint(l.start) + " to " + genPoint(l.end);
        }
        public void handle(twoLines d)
        {
            Console.WriteLine("Got twoLines: ");
            Console.WriteLine("Line l1: "+genLine(d.l1));
            Console.WriteLine("Line l2: " + genLine(d.l2));
        }

        static void Main(string[] args)
        {
            new Decoder(new FileStream(args[0], FileMode.Open));
        }
    }
}
