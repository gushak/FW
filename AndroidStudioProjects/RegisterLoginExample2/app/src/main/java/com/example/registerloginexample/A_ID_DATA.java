package com.example.registerloginexample;


import android.os.Parcel;
import android.os.Parcelable;

public class A_ID_DATA implements Parcelable {

    public int RU_ID;
    public int CASCADE_ID;
    public byte LAYER_USE_INFO;

    public short PCI_100M;
    public byte Cell_id_100M;
    public int Gnb_id_100M;
    public int Ls_id_100M;
    public byte PortNo_100M;
    public byte ChBw_100M;
    public float CenFreq_100M;

    public short PCI_a;
    public byte Cell_id_a;
    public int Gnb_id_a;
    public int Ls_id_a;
    public byte PortNo_a;
    public byte ChBw_a;
    public float CenFreq_a;

    public byte SW_VER[] = new byte[3];
    public byte MANU;
    public float RU_TEMP= 0.1f;
    public float RU_TEMP_MAX = 0.1f;
    public byte DL_STATE[] = new byte[2];
    public byte RU_INIT_STATE;
    public int LS_MANU;
    public byte MISMATCH_ID;

    public byte TREE_INFO[] = new byte[6];

    public byte RuType;

    public A_ID_DATA(Parcel parcel) {
        this.RU_ID = parcel.readInt();
        this.CASCADE_ID = parcel.readInt();
        this.LAYER_USE_INFO = parcel.readByte();

        this.PCI_100M = (short)parcel.readInt();
        this.Cell_id_100M = parcel.readByte();
        this.Gnb_id_100M = parcel.readInt();
        this.Ls_id_100M = parcel.readInt();
        this.PortNo_100M = parcel.readByte();
        this.ChBw_100M = parcel.readByte();
        this.CenFreq_100M = parcel.readInt();

        this.PCI_a = (short)parcel.readInt();
        this.Cell_id_a = parcel.readByte();
        this.Gnb_id_a = parcel.readInt();
        this.Ls_id_a = parcel.readInt();
        this.PortNo_a = parcel.readByte();
        this.ChBw_a = parcel.readByte();
        this.CenFreq_a = parcel.readInt();

        this.SW_VER[0] = parcel.readByte();
        this.SW_VER[1] = parcel.readByte();
        this.SW_VER[2] = parcel.readByte();
        this.MANU = parcel.readByte();
        this.RU_TEMP = parcel.readFloat();
        this.RU_TEMP_MAX = parcel.readFloat();

        this.DL_STATE[0] = parcel.readByte();
        this.DL_STATE[1] = parcel.readByte();


        this.RU_INIT_STATE = parcel.readByte();
        this.LS_MANU = parcel.readInt();
        this.MISMATCH_ID  = parcel.readByte();

        this.TREE_INFO[0] = parcel.readByte();
        this.TREE_INFO[1] = parcel.readByte();
        this.TREE_INFO[2] = parcel.readByte();
        this.TREE_INFO[3] = parcel.readByte();
        this.TREE_INFO[4] = parcel.readByte();
        this.TREE_INFO[5] = parcel.readByte();

        this.RuType = parcel.readByte();
    }

    public  A_ID_DATA(){

    }

    public static final Parcelable.Creator<A_ID_DATA> CREATOR = new Parcelable.Creator<A_ID_DATA>(){
        @Override
        public A_ID_DATA createFromParcel(Parcel parcel){
            return new A_ID_DATA(parcel);
        }
        @Override
        public A_ID_DATA[] newArray(int size){
            return new A_ID_DATA[size];
        }
    };

    @Override
    public void writeToParcel(Parcel dest, int flags){
        dest.writeInt(this.RU_ID);
        dest.writeInt(this.CASCADE_ID);
        dest.writeByte(this.LAYER_USE_INFO);

        dest.writeInt((int)this.PCI_100M);
        dest.writeByte(this.Cell_id_100M);
        dest.writeInt(this.Gnb_id_100M);
        dest.writeInt(this.Ls_id_100M);
        dest.writeByte(this.PortNo_100M);
        dest.writeByte(this.ChBw_100M);
        dest.writeFloat(this.CenFreq_100M);

        dest.writeInt((int)this.PCI_a);
        dest.writeByte(this.Cell_id_a);
        dest.writeInt(this.Gnb_id_a);
        dest.writeInt(this.Ls_id_a);
        dest.writeByte(this.PortNo_a);
        dest.writeByte(this.ChBw_a);
        dest.writeFloat(this.CenFreq_a);

        dest.writeByte(this.SW_VER[0]);
        dest.writeByte(this.SW_VER[1]);
        dest.writeByte(this.SW_VER[2]);

        dest.writeByte(this.MANU);
        dest.writeFloat(this.RU_TEMP);
        dest.writeFloat(this.RU_TEMP_MAX);
        dest.writeByte(this.DL_STATE[0]);
        dest.writeByte(this.DL_STATE[1]);

        dest.writeByte(this.RU_INIT_STATE);
        dest.writeInt(this.LS_MANU);
        dest.writeByte(this.MISMATCH_ID);

        dest.writeByte(this.TREE_INFO[0]);
        dest.writeByte(this.TREE_INFO[1]);
        dest.writeByte(this.TREE_INFO[2]);
        dest.writeByte(this.TREE_INFO[3]);
        dest.writeByte(this.TREE_INFO[4]);
        dest.writeByte(this.TREE_INFO[5]);

        dest.writeByte(this.RuType);
    }

    @Override
    public int describeContents(){
        return 0;
    }

    public int getRU_ID(){
        return this.RU_ID;
    }

    public byte[] getSW_VER(){
        return this.SW_VER;
    }
}