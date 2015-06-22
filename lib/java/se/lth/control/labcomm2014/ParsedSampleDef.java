package se.lth.control.labcomm2014;

public class ParsedSampleDef extends ParsedTypeDef{
    int idx;

    ParsedSampleDef(ParsedTypeDef td) {
        super(td);
    }

    @Override
    public boolean isSampleDef() {
        return true;
    }

    public String defType() {
        return "sample";
    }

}


