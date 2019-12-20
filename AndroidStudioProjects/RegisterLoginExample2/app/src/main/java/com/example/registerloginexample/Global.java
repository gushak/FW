package com.example.registerloginexample;

public class Global {

    private static int gPhase = 0;
    private static boolean gTxReset = false;
    private static float gCenFreq_100M = 0;
    private static float gCenFreq_a = 0;

    public static boolean GetTxReset()
    {
        return gTxReset;
    }
    public static int GetActPhase(){return gPhase;}
    public static float GetCenFreq_100M(){return gCenFreq_100M;}
    public static float GetCenFreq_a(){return gCenFreq_a;}

    public static void SetActPhase(int phase){
        gPhase = phase;
    }

    public static void SetTxReset(boolean data){
        gTxReset = data;
    }

    public static void SetCenFreq_100M(float freq)
    {
        gCenFreq_100M = freq;
    }
    public static void SetCenFreq_a(float freq)
    {
        gCenFreq_a = freq;
    }
    private static Global instance = null;
    public static synchronized Global getInstance()
    {
        if(null == instance){
            instance = new Global();
        }
        return instance;
    }
}
