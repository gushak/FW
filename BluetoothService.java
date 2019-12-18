package com.example.registerloginexample;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;

import androidx.annotation.Nullable;

import java.util.List;
import java.util.Set;
import java.util.UUID;


public class BluetoothService extends Service {

    final static UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    //=================PUBLIC A_ID====================
    static final short AID_ID = 0x5000;
    static final short AID_CASCADE_ID = 0x5001;
    static final short AID_LAYER_USE_INFO = 0x5002;
    static final short AID_PCI = 0x5003;
    static final short AID_CELL_ID = 0x5004;
    static final short AID_GNB_ID = 0x5005;
    static final short AID_PORT_NO = 0x5006;
    static final short AID_CH_BW = 0x5008;
    static final short AID_LS_MANU = 0x500A;
    static final short AID_CENTER_FRQ = 0x500B;
    static final short AID_LS_ID = 0x500C;
    static final short AID_RU_INIT_ST = 0x500D;
    static final short AID_COND_WATCH_DEVICE_CON = 0x5010;
    static final short AID_DOOR = 0x5011;
    static final short AID_RU_TEMP = 0x5012;
    static final short AID_RU_TEMP_MAX = 0x5013;
    static final short AID_FAN1 = 0x5014;
    static final short AID_DC = 0x5017;
    static final short AID_BATTERY_LOW = 0x5018;

    //DL
    static final short AID_DL_TSSI = 0x5100;
    static final short AID_DL_TSSI_MAX_ONE = 0x5101;
    static final short AID_DL_TSSI_MAX_TWO = 0x5102;
    static final short AID_DL_OUTPUT = 0x5103;
    static final short AID_DL_OUTPUT_MAX = 0x5104;
    static final short AID_DL_OUTPUT_MIN = 0x5105;
    static final short AID_DL_ATTN = 0x5106;
    static final short AID_DL_TSSI_DEC = 0x5107;
    static final short AID_DL_OUTPUT_CAL = 0x5108;
    static final short AID_OVR_PWR = 0x5110;
    static final short AID_SHUTDOWN_DL = 0x5115;
    static final short AID_PAU_ONOFF = 0x5116;
    static final short AID_DL_DISABLED = 0x5117;

    //UL
    static final short AID_UL_RSSI = 0x5200;
    static final short AID_UL_RSSI_MAX = 0x5201;
    static final short AID_UL_ATTN = 0x5206;
    static final short AID_UL_GAIN = 0x5207;
    static final short AID_UL_GAIN_BACKOFF = 0x5208;
    static final short AID_UL_GAIN_BALANCE = 0x5209;
    static final short AID_UL_OVR_IN = 0x5210;
    static final short AID_UL_VSWR = 0x5211;
    static final short AID_UL_GAIN_LO = 0x5213;
    static final short AID_UL_LNA_ONOFF = 0x5216;

    //DTU&SFP
    static final short AID_CLK = 0x5300;
    static final short AID_CPRI = 0x5301;
    static final short AID_SFP_LOS = 0x5302;
    static final short AID_CPRI_LOF = 0x5303;
    static final short AID_SFP_LD = 0x5304;
    static final short AID_CPRI_LD_PWR = 0x5305;
    static final short AID_CPRI_PD_PWR = 0x5308;
    static final short AID_CPRI_BIP_ERR_CNT = 0x530A;
    static final short AID_CPRI_SFP_VCC = 0x5310;
    static final short AID_CPRI_LD_BIAS = 0x5311;
    static final short AID_CPRI_WAVE = 0x5312;
    static final short AID_CPRI_BITRATE = 0x5313;
    static final short AID_CPRI_SFP_TMP = 0x5314;
    static final short AID_SFP_TAKEN = 0x5315;
    static final short AID_DL_HARD_CLIPING = 0x5320;

    static final short AID_TOFFSET = 0x5400;
    static final short AID_TB_DELAY_DL = 0x5401;
    static final short AID_TB_DELAY_UL = 0x5402;
    static final short AID_T2A_DL_DELAY = 0x5403;
    static final short AD_TA3_UL_DELAY = 0x5404;
    static final short AID_OPTIC_DELAY = 0x5405;
    static final short AID_DLUL_VERIF = 0x5408;
    static final short AID_FAN_MODE = 0x5410;
    static final short AID_FAN_MODE_ACT_ST = 0x5411;
    static final short AID_FAN_MODE_LO_NOISE_ACT_T = 0x5412;
    static final short AID_TDD_MODE = 0x5420;
    static final short AID_DL_ON_GUARD_T = 0x5421;
    static final short AID_DL_OFF_GUARD_T = 0x5422;
    static final short AID_DL_UL_GUARD_T = 0x5423;
    static final short AID_UL_DL_GUARD_T = 0x5424;
    static final short AID_MIMO_STAND_MODE_ONOFF = 0x5430;
    static final short AID_MIMO_STAND_MODE_OP_ST = 0x5431;
    static final short AID_MIMO_STAN_MODE_OP_T = 0x5432;
    static final short AID_LS_INDI_OP_MODE = 0x5433;
    static final short AID_STAND_MODE_ONOFF = 0x5434;
    static final short AID_STAND_MODE_OP_ST = 0x5435;
    static final short AID_STAND_MODE_OP_T = 0x5436;
    static final short AID_STAND_MODE_APPLY_PATH = 0x5437;
    static final short AID_MIMO_STAND_MODE_ENTER_LEVEL = 0x543A;
    static final short AID_MIMO_STAND_MODE_ENTER_RELEASE = 0x543B;
    static final short AID_UDA_1 = 0x5440; //translated to English
    static final short AID_UDA_CLK = 0x5442;
    static final short AID_CPRI_PD = 0x5443;
    static final short AID_UDA_1_OP_MODE = 0x5444; //translated to English
    static final short AID_UDA_CLK_OP_MODE = 0x5446;
    static final short AID_UDA_PD_OP_MODE = 0x5447;
    static final short AID_UDA_CLK_RESET_SUP = 0x544A;
    static final short AID_UDA_CPIRI_RESET_SUP = 0x544B;
    static final short AID_UDA_1_OP_LAYER = 0x544C; //translated to English
    static final short AID_UL_GAIN_AUTO_BACKOFF = 0x5450;
    static final short AID_UL_GAIN_WATCH = 0x5451;
    static final short AID_LOGGING_ONOFF = 0x5453;
    static final short AID_LOG_CONT = 0x5454;
    static final short AID_LED_AUTO_OFF = 0x5455;

    //Configuration
    static final short AID_MANU = 0x5500;
    static final short AID_SUPPLIER = 0x5501;
    static final short AID_TYPE = 0x5502;
    static final short AID_SW_VER = 0x5503;

    //etc
    static final short AID_STATISTICS_REF_T = 0x5600;
    static final short AID_STATISTICS_INTERVAL = 0x5601;
    static final short AID_STATISTICS_INIT_T = 0x5601;
    static final short AID_AUTO_STATISTICS_REF_T = 0x5603;
    static final short AID_DOWNLOAD_STAT = 0x5610;
    static final short AID_ID_MISMATCH = 0x5611;
    static final short AID_RESET = 0x5620;

    //===============Hidden A_ID=========================
    static final int AID_CONF_ENV_TREE = 0xB00F;


    static BluetoothAdapter mBluetoothAdapter;
    Set<BluetoothDevice> mPairedDevices;
    List<String> mListPairedDevices;


    Handler mBluetoothHandler;

    static BluetoothService.ConnectedBluetoothThread mThreadConnectedBluetooth;
    BluetoothDevice mBluetoothDevice;
    BluetoothSocket mBluetoothSocket;

    gRemsProtocol gRemsPro = new gRemsProtocol();
    //    gRemmsPacket gRemsPack = new gRemmsPacket();
    gRems_Macro Macro = new gRems_Macro();

    final static int BT_REQUEST_ENABLE = 1;
    final static int BT_MESSAGE_READ = 2;
    final static int BT_CONNECTING_STATUS = 3;


    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }


    public int onStartCommand(Intent intent, int flags, int startId) {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        return startId;

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }


    @Override
    public void onCreate() {
        super.onCreate();

    };
}

