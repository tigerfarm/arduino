/*
 * Pretty print, print nicely formatted XML data.
 */
package asm;

public class PrintXml {

    public void pretty(String xmlData) {
        int si;
        int ei;
        int noDataItem;
        int endDataItem;
        int nextDataItem;
        int responseItem;
        String tabString = "  ";
        int tabStringLength = tabString.length();
        String indentString = "";

        String theClassName = "PrintXml";
        System.out.println("+++ Start class: " + theClassName);
        // System.out.println("++ Print: " + xmlData);
        si = xmlData.indexOf("<", 0);
        ei = xmlData.indexOf(">", 0);
        if (si < 0 || ei < 0) {
            System.out.println("-- Error: responce is Not XML data.");
            System.out.println("+ Responce text: " + xmlData + "...");
            return;
        }
        while (ei > 0) {
            noDataItem = xmlData.indexOf("/>", si);
            responseItem = xmlData.indexOf("<?", si);
            endDataItem = xmlData.indexOf("</", si);
            nextDataItem = xmlData.indexOf("<", ei+1);
            // System.out.println("si="+si+", ei="+ei+", endDataItem="+endDataItem + ", nextDataItem="+nextDataItem);
            if (ei == noDataItem + 1) {
                // Case, no data in the item: <soapenv:Header/>
                System.out.println(indentString + xmlData.substring(si, ei + 1));
            } else if (responseItem==0) {
                // Case, response heading: <?xml version='1.0' encoding='UTF-8'?>
                System.out.println(indentString + xmlData.substring(si, ei + 1));
            } else if (si == endDataItem) {
                // Case, data item: ></v2:values>
                indentString = indentString.substring(0, indentString.length()-tabStringLength);
                System.out.println(indentString + xmlData.substring(si, ei + 1));
            } else if (ei == nextDataItem - 1) {
                // Case, data item: <soapenv:Body><v2:runReport>
                System.out.println(indentString + xmlData.substring(si, ei + 1));
                indentString += tabString;
            } else {
                // Case, data item: <v2:attributeFormat>csv</v2:attributeFormat>
                ei = xmlData.indexOf(">", nextDataItem);
                System.out.println(indentString + xmlData.substring(si, ei + 1));
            }
            si = ei + 1;
            ei = xmlData.indexOf(">", si);
        }
        System.out.println("+++ Exit class: " + theClassName + ".");
    }

}
