package se.lth.control.labcomm2014;

public class SigSampleDef extends SigTypeDef{


    SigSampleDef(SigTypeDef td) {
        super(td.getName(), td.getType());
    }

    @Override
    public boolean isSampleDef() {
        return true;
    }

    public String defType() {
        return "sample";
    }

}


