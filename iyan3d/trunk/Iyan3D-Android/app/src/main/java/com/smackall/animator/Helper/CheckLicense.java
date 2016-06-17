package com.smackall.animator.Helper;

import android.content.Context;


public class CheckLicense {

    static String CERTIFICATE = "A4";
    static String CERTIFICATE1 = "A5";
    static String CERTIFICATE2 = "6C";
    static String CERTIFICATE3 = "78";
    static String CERTIFICATE4 = "8E";
    static String CERTIFICATE5 = "8E";
    static String CERTIFICATE6 = "FE";
    static String CERTIFICATE7 = "67";
    static String CERTIFICATE8 = "C6";
    static String CERTIFICATE9 = "9C";
    static String CERTIFICATE10 = "EB";
    static String CERTIFICATE11 = "06";
    static String CERTIFICATE12 = "70";
    static String CERTIFICATE13 = "3E";
    static String CERTIFICATE14 = "D9";
    static String CERTIFICATE15 = "FC";
    static String CERTIFICATE16 = "6C";
    static String CERTIFICATE17 = "58";
    static String CERTIFICATE18 = "4D";
    static String CERTIFICATE19 = "87";

    public static boolean checkLicense(Context mContext) {
       String CERTIFICATE_ORG = CERTIFICATE + ":" + CERTIFICATE1 + ":" + CERTIFICATE2 + ":" + CERTIFICATE3 + ":" + CERTIFICATE4 + ":" + CERTIFICATE5 + ":" + CERTIFICATE6 + ":"
               + CERTIFICATE7 + ":" + CERTIFICATE8 + ":" + CERTIFICATE9 + ":" + CERTIFICATE10 + ":" + CERTIFICATE11 + ":" + CERTIFICATE12 + ":" + CERTIFICATE13 + ":" + CERTIFICATE14 + ":" + CERTIFICATE15 + ":"
               + CERTIFICATE16 + ":" + CERTIFICATE17 + ":" + CERTIFICATE18 + ":" + CERTIFICATE19;
        return CERTIFICATE_ORG.equals(CheckCertificate.getCertificateSHA1Fingerprint(mContext));
    }
}

