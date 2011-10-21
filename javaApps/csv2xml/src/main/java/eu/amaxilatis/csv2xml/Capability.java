package eu.amaxilatis.csv2xml;


class Capability {

    private String name_;
    private String datatype_;
    private String unit_;
    private String default_;

    public Capability(String tname, String tdatatype, String tunit, String tdefault) {
        name_ = tname;
        datatype_ = tdatatype;
        unit_ = tunit;
        default_ = tdefault;
    }


    public String getDescription() {
        return    "\t\t\t<capability>\n"
                + "\t\t\t\t<name>" + name_ + "</name>\n"
                + "\t\t\t\t<datatype>" + datatype_ + "</datatype>\n"
                + "\t\t\t\t<unit>" + unit_ + "</unit>\n"
                + "\t\t\t\t<default>" + default_ + "</default>\n"
                + "\t\t\t</capability>\n";
    }


}