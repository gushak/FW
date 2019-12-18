package com.example.registerloginexample;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Set;
import java.util.UUID;

public class CascadeActivity extends AppCompatActivity {

    TextView mTvBluetoothStatus;
    TextView mTvGuiCon_1;

    ImageView mImgAAU_White_1;
    ImageView mImgAAU_White_2;
    ImageView mImgAAU_White_3;
    ImageView mImgAAU_White_4;


    ImageView mImgAAU_Black_1;
    ImageView mImgAAU_Black_2;
    ImageView mImgAAU_Black_3;
    ImageView mImgAAU_Black_4;

    Button mBtnBluetoothOn;
    Button mBtnBluetoothOff;
    Button mBtnConnect;

    int CRC_16_POLYNOMIAL = 0x1021;
    int CRC_16_SEED = 0x0000;
    int CRC_16_STEP_SEED = ~((int) CRC_16_SEED);
    int CRC_TAB_SIZE = 256;

    final static UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    int[] Crc16_Table = {
            0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
            0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
            0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
            0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
            0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
            0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
            0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
            0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
            0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
            0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
            0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
            0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
            0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
            0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
            0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
            0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
            0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
            0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
            0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
            0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
            0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
            0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
            0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
            0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
            0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
            0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
            0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
            0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
            0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
            0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
            0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
            0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};


    A_ID_DATA A_id_data = new A_ID_DATA();

    static byte[] RxByte = new byte[8096];

    Thread Tx_thread;
    Thread Bt_thread;
    String CurrentBT;

    boolean IsConnectedBT = false;
    boolean mTransceiver = false;
    byte mCarreirCnt = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cascade);

        mBtnBluetoothOn = (Button) findViewById(R.id.btnBluetoothOn);
        mBtnBluetoothOff = (Button) findViewById(R.id.btnBluetoothOff);
        mBtnConnect = (Button) findViewById(R.id.btnConnect);

        mTvBluetoothStatus = (TextView)findViewById(R.id.tvBluetoothStatus);
        mTvGuiCon_1 = (TextView)findViewById(R.id.tvGuiCon_1);

        mImgAAU_Black_1 = (ImageView)findViewById(R.id.AAU_Black_1);
        mImgAAU_White_1 = (ImageView)findViewById(R.id.AAU_White_1);
        mImgAAU_Black_2 = (ImageView)findViewById(R.id.AAU_Black_2);
        mImgAAU_White_2 = (ImageView)findViewById(R.id.AAU_White_2);
        mImgAAU_Black_3 = (ImageView)findViewById(R.id.AAU_Black_3);
        mImgAAU_White_3 = (ImageView)findViewById(R.id.AAU_White_3);
        mImgAAU_Black_4 = (ImageView)findViewById(R.id.AAU_Black_4);
        mImgAAU_White_4 = (ImageView)findViewById(R.id.AAU_White_4);


        mImgAAU_Black_1.setOnClickListener(new Button.OnClickListener(){

            @Override
            public void onClick(View view) {
                Toast.makeText(getApplicationContext(), "미 사용", Toast.LENGTH_SHORT).show();
               }
        });

        mImgAAU_White_1.setOnClickListener(new Button.OnClickListener(){

            @Override
            public void onClick(View view) {
                Toast.makeText(getApplicationContext(), "5G GUI에 접속하였습니다.", Toast.LENGTH_SHORT).show();
                Intent intent = new Intent(CascadeActivity.this, MainActivity.class);
                intent.putExtra("Intent_Aid", A_id_data);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
            }
        });

        mBtnBluetoothOn.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                    IsConnectedBT = false;
                    bluetoothOn();
            }
        });

        mBtnBluetoothOff.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                bluetoothOff();
            }
        });
        mBtnConnect.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (IsConnectedBT == true) {
                    if (!mBluetoothAdapter.isEnabled()) {
                        Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
                        }
                    }
                    IsConnectedBT = false;
                    listPairedDevices();
            }
        });

        mBluetoothHandler = new Handler() {
            public synchronized void handleMessage(android.os.Message msg) {
                if (msg.what == BT_MESSAGE_READ) {
                    RxByte = (byte[]) msg.obj;

                    if(A_id_data.TREE_INFO[Macro.RU_ONE_STATE] == 1){
                        mImgAAU_White_1.setVisibility(View.VISIBLE);
                        mImgAAU_Black_1.setVisibility(View.INVISIBLE);
                    }
                    else{
                        mImgAAU_White_1.setVisibility(View.INVISIBLE);
                        mImgAAU_Black_1.setVisibility(View.VISIBLE);
                    }

                    if(A_id_data.TREE_INFO[Macro.RU_TWO_STATE] == 1){
                        mImgAAU_White_2.setVisibility(View.VISIBLE);
                        mImgAAU_Black_2.setVisibility(View.INVISIBLE);
                    }
                    else{
                        mImgAAU_White_2.setVisibility(View.INVISIBLE);
                        mImgAAU_Black_2.setVisibility(View.VISIBLE);
                    }

                    if(A_id_data.TREE_INFO[Macro.RU_THREE_STATE] == 1){
                        mImgAAU_White_3.setVisibility(View.VISIBLE);
                        mImgAAU_Black_3.setVisibility(View.INVISIBLE);
                    }
                    else{
                        mImgAAU_White_3.setVisibility(View.INVISIBLE);
                        mImgAAU_Black_3.setVisibility(View.VISIBLE);
                    }

                    if(A_id_data.TREE_INFO[Macro.RU_FOUR_STATE] == 1){
                        mImgAAU_White_4.setVisibility(View.VISIBLE);
                        mImgAAU_Black_4.setVisibility(View.INVISIBLE);
                    }
                    else{
                        mImgAAU_White_4.setVisibility(View.INVISIBLE);
                        mImgAAU_Black_4.setVisibility(View.VISIBLE);
                    }

                    if(A_id_data.TREE_INFO[Macro.RU_ONE_STATE] == 1 &&A_id_data.TREE_INFO[Macro.RU_TWO_STATE] == 1 &&
                            A_id_data.TREE_INFO[Macro.RU_THREE_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_FOUR_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_CON_GUI] == 0)
                    {
                        mTvGuiCon_1.setText("GUI 연결");
                        mTvGuiCon_1.setTranslationX(0);
                        mTvGuiCon_1.setTranslationY(0);
                    }
                    else if(A_id_data.TREE_INFO[Macro.RU_ONE_STATE] == 1 &&A_id_data.TREE_INFO[Macro.RU_TWO_STATE] == 1 &&
                            A_id_data.TREE_INFO[Macro.RU_THREE_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_FOUR_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_CON_GUI] == 1)
                    {
                        mTvGuiCon_1.setText("GUI 연결");
                        mTvGuiCon_1.setTranslationX(145);
                    }
                    else if(A_id_data.TREE_INFO[Macro.RU_ONE_STATE] == 1 &&A_id_data.TREE_INFO[Macro.RU_TWO_STATE] == 1 &&
                            A_id_data.TREE_INFO[Macro.RU_THREE_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_FOUR_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_CON_GUI] == 2)
                    {
                        mTvGuiCon_1.setText("GUI 연결");
                        mTvGuiCon_1.setTranslationX(0);
                        mTvGuiCon_1.setTranslationY(145);
                    }
                    else if(A_id_data.TREE_INFO[Macro.RU_ONE_STATE] == 1 &&A_id_data.TREE_INFO[Macro.RU_TWO_STATE] == 1 &&
                            A_id_data.TREE_INFO[Macro.RU_THREE_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_FOUR_STATE] == 1 && A_id_data.TREE_INFO[Macro.RU_CON_GUI] == 3)
                    {
                        mTvGuiCon_1.setText("GUI 연결");
                        mTvGuiCon_1.setTranslationX(145);
                        mTvGuiCon_1.setTranslationY(145);
                    }
                }
            }
        };
    }

    final Handler bt_handler = new Handler(){
        @Override
        public void handleMessage(@NonNull Message msg) {
            if (IsConnectedBT == true){
                if (!mBluetoothAdapter.isEnabled()) {
                    Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
                }
            }
        }
    };

        public static void tx_blutooth(byte[] data){
            if (mBluetoothAdapter.isEnabled()) {
                for (int i = 0; i < data.length; i++)
                    mThreadConnectedBluetooth.write(data[i]);
            }
        }

        void bluetoothOn() {
            if(mBluetoothAdapter == null) {
                Toast.makeText(getApplicationContext(), "블루투스를 지원하지 않는 기기입니다.", Toast.LENGTH_LONG).show();
                IsConnectedBT = false;
            }
            else {
                if (mBluetoothAdapter.isEnabled()) {
                    Toast.makeText(getApplicationContext(), "블루투스가 이미 활성화 되어 있습니다.", Toast.LENGTH_LONG).show();
                    mTvBluetoothStatus.setText("활성화");
                }
                else {
                    Toast.makeText(getApplicationContext(), "블루투스가 활성화 되어 있지 않습니다.", Toast.LENGTH_LONG).show();
                    Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    IsConnectedBT = false;
                    startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
                }
            }
        }
        void bluetoothOff() {
            if (mBluetoothAdapter.isEnabled()) {
                mBluetoothAdapter.disable();
                Toast.makeText(getApplicationContext(), "블루투스가 비활성화 되었습니다.", Toast.LENGTH_SHORT).show();
                mTvBluetoothStatus.setText("비활성화");
                IsConnectedBT = false;
                System.out.println("블루투스 비활성화");
            }
            else {
                Toast.makeText(getApplicationContext(), "블루투스가 이미 비활성화 되어 있습니다.", Toast.LENGTH_SHORT).show();
            }
        }
        @Override
        protected void onActivityResult(int requestCode, int resultCode, Intent data) {
            switch (requestCode) {
                case BT_REQUEST_ENABLE:
                    if (resultCode == RESULT_OK) { // 블루투스 활성화를 확인을 클릭하였다면
                        Toast.makeText(getApplicationContext(), "블루투스 활성화", Toast.LENGTH_LONG).show();
                        mTvBluetoothStatus.setText("활성화");
                    } else if (resultCode == RESULT_CANCELED) { // 블루투스 활성화를 취소를 클릭하였다면
                        Toast.makeText(getApplicationContext(), "취소", Toast.LENGTH_LONG).show();
                        mTvBluetoothStatus.setText("비활성화");
                    }
                    break;
            }
            super.onActivityResult(requestCode, resultCode, data);
        }
        void listPairedDevices() {
            if (mBluetoothAdapter.isEnabled()) {
                mPairedDevices = mBluetoothAdapter.getBondedDevices();

                if (mPairedDevices.size() > 0) {
                    AlertDialog.Builder builder = new AlertDialog.Builder(this);
                    builder.setTitle("장치 선택");

                    mListPairedDevices = new ArrayList<String>();
                    for (BluetoothDevice device : mPairedDevices) {
                        mListPairedDevices.add(device.getName());
                        //mListPairedDevices.add(device.getName() + "\n" + device.getAddress());
                    }
                    final CharSequence[] items = mListPairedDevices.toArray(new CharSequence[mListPairedDevices.size()]);
                    mListPairedDevices.toArray(new CharSequence[mListPairedDevices.size()]);

                    builder.setItems(items, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int item) {
                            connectSelectedDevice(items[item].toString());
                            CurrentBT = items[item].toString();
                        }
                    });
                    AlertDialog alert = builder.create();
                    alert.show();
                } else {
                    Toast.makeText(getApplicationContext(), "페어링된 장치가 없습니다.", Toast.LENGTH_LONG).show();
                }
            }
            else {
                Toast.makeText(getApplicationContext(), "블루투스가 비활성화 되어 있습니다.", Toast.LENGTH_SHORT).show();
            }
        }
        void connectSelectedDevice(String selectedDeviceName) {
            for(BluetoothDevice tempDevice : mPairedDevices) {
                if (selectedDeviceName.equals(tempDevice.getName())) {
                    mBluetoothDevice = tempDevice;
                    break;
                }
            }
            try {
                mBluetoothSocket = mBluetoothDevice.createRfcommSocketToServiceRecord(BT_UUID);
                mBluetoothSocket.connect();
                mThreadConnectedBluetooth = new CascadeActivity.ConnectedBluetoothThread(mBluetoothSocket);

                mThreadConnectedBluetooth.start();
                mBluetoothHandler.obtainMessage(BT_CONNECTING_STATUS, 1, -1).sendToTarget();
                IsConnectedBT = true;
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "블루투스 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }

        void ReConnectDevice() {
            try {
                mBluetoothSocket = mBluetoothDevice.createRfcommSocketToServiceRecord(BT_UUID);
                mBluetoothSocket.connect();
                mThreadConnectedBluetooth = new CascadeActivity.ConnectedBluetoothThread(mBluetoothSocket);
                mThreadConnectedBluetooth.start();
                mBluetoothHandler.obtainMessage(BT_CONNECTING_STATUS, 1, -1).sendToTarget();
                IsConnectedBT = true;
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "블루투스 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }


        private class ConnectedBluetoothThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;
        private Queue<byte[]> bufferQueue = new LinkedList<byte[]>();

        public ConnectedBluetoothThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
                System.out.println("소켓 불량");
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }
        public void run() {

            final byte TX_STAT_REF[] = new byte[]{0x16, 0x16, 0x16, 0x16, 0x00, 0x0c, 0x01, 0x01, 0x00, 0x09, 0x17, (byte) 0xff, (byte) 0xff, (byte) 0xff, 0x00, 0x50, 0x00, 0x00, (byte) 0xf5, (byte) 0xaf, 0x03};
            final byte TxReset[] = {0x16, 0x16, 0x16, 0x16, 0x5F, (byte)0xC4, 0x01, 0x01, 0x00, 0x15, 0x3A, (byte)0xFF, (byte)0xFF, (byte)0xFF, 0x01, 0x50, 0x00, 0x0C, 0x56, 0x20, 0x00, 0x08 ,
                    0x09 , 0x00, 0x00, 0x01, 0x01, 0x01, (byte)0x80, 0x01, (byte)0xF5 ,0x4E, 0x03 };
            byte[] buffer = new byte[8096];
            int bytes;

            while (true) {
                try {
                    if (IsConnectedBT == true) {
                        if (!mBluetoothAdapter.isEnabled()) {
                            Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                            startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
                        } else {
                            tx_blutooth(TX_STAT_REF);
                            mTransceiver =  true;
                        }
                        SystemClock.sleep(100);

                        if(Global.GetTxReset())
                        {
                            tx_blutooth(TxReset);
                            Global.SetTxReset(false);
                            SystemClock.sleep(60000);

                        }

                        System.out.println("스레드 동작중 ");
                    }

                    bytes = mmInStream.available();
                    if (bytes != 0) {
                        SystemClock.sleep(100);
                        bytes = mmInStream.available();
                        bytes = mmInStream.read(buffer);
//                       bufferQueue.offer(new byte[bytes]);
//                       System.arraycopy(buffer, 0, bufferQueue.peek(), 0, bytes);
                        Message msg = new Message();
                        msg.what = BT_MESSAGE_READ;
                        msg.obj = bufferQueue.poll();
                        msg.arg1 = bytes;
                        int Count = 0;
                        int ParseIndex = 0;

                        if(mTransceiver == true) {
                            if (buffer != null && buffer[0] == Macro.STX) {
                                for (ParseIndex = 0; ParseIndex < buffer.length; ParseIndex++) {

                                    if (buffer[ParseIndex] == Macro.STX && Count == 0) {
                                        Count++;
                                    } else if (buffer[ParseIndex] == Macro.STX && Count == 1) {
                                        Count++;
                                    } else if (buffer[ParseIndex] == Macro.STX && Count == 2) {
                                        Count++;
                                    } else if (buffer[ParseIndex] == Macro.STX && Count == 3) {
                                        Count++;
                                    } else if (Count >= Macro.HEAD_LEN) {
                                        gRemsPro.SeqNum += ((buffer[ParseIndex++] << 8) & 0xFF00);
                                        gRemsPro.SeqNum += buffer[ParseIndex++];
                                        gRemsPro.ToTLenPacket = buffer[ParseIndex++];
                                        gRemsPro.CunrLenPacket = buffer[ParseIndex++];
                                        if (gRemsPro.ToTLenPacket >= gRemsPro.CunrLenPacket) {
                                            gRemsPro.ToTBodyLen += ((buffer[ParseIndex++] << 8) & 0xFF00);
                                            gRemsPro.ToTBodyLen += buffer[ParseIndex++];
                                            gRemsPro.InfoCheckSum = buffer[ParseIndex++];

                                            byte CalCheckSum = 0;

                                            for (int crcindex = 4; crcindex < ParseIndex - 1; crcindex++) {
                                                CalCheckSum += buffer[crcindex];
                                                if (CalCheckSum == gRemsPro.InfoCheckSum) {
                                                    break;
                                                }
                                            }
                                            if (CalCheckSum == gRemsPro.InfoCheckSum) {

                                                gRemsPro.Body.SUB_ID += buffer[ParseIndex++];
                                                gRemsPro.Body.SUB_ID += buffer[ParseIndex++];
                                                gRemsPro.Body.SUB_ID += buffer[ParseIndex++];

                                                gRemsPro.Body.CMD = buffer[ParseIndex++];
                                                gRemsPro.Body.RCODE = buffer[ParseIndex++];
                                                gRemsPro.Body.LEN += (0xFF00 & buffer[ParseIndex++] << 8);
                                                gRemsPro.Body.LEN += buffer[ParseIndex++];

                                                int bodyindex = 0;

                                                for (bodyindex = ParseIndex; bodyindex < gRemsPro.ToTBodyLen; bodyindex++) {

                                                    if (buffer[gRemsPro.ToTBodyLen + 1] == Macro.ETX)
                                                        break;
                                                    if (bodyindex == 0x1F90)
                                                        break;

                                                    gRemsPro.Body.A_ID.A_ID = 0;
                                                    gRemsPro.Body.A_ID.A_ID += (buffer[bodyindex] << 8 & 0xFF00);
                                                    gRemsPro.Body.A_ID.A_ID += buffer[bodyindex + 1];

//                                                System.out.println("바디 인덱스");
//                                                System.out.println(bodyindex);

                                                    if ((gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x50 || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x51 || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x52 || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x53 || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x54
                                                            || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x55 || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0x56 || (gRemsPro.Body.A_ID.A_ID >> 8 & 0xFF) == 0xB0){ //유효성 검사 필요
                                                        bodyindex += 2;
                                                        gRemsPro.Body.A_ID.LEN = 0;
                                                        gRemsPro.Body.A_ID.LEN += (buffer[bodyindex++] << 8) & 0xFF00;
                                                        gRemsPro.Body.A_ID.LEN += buffer[bodyindex++];
                                                        if (gRemsPro.Body.A_ID.LEN < 0x00 || 0xFF < gRemsPro.Body.A_ID.LEN) {
                                                            continue;
                                                        }


                                                        gRemsPro.Body.A_ID.DataType = buffer[bodyindex++];

                                                        if (gRemsPro.Body.A_ID.DataType <= 0x00 || 0x0A <= gRemsPro.Body.A_ID.DataType) {
                                                            gRemsPro.Body.A_ID.DataType = 0;
                                                            continue;
                                                        }


                                                        gRemsPro.Body.A_ID.DataStep = buffer[bodyindex++];
                                                        gRemsPro.Body.A_ID.Reserved = buffer[bodyindex++];
                                                        gRemsPro.Body.A_ID.DATA_LEN = buffer[bodyindex++];

                                                        if (gRemsPro.Body.A_ID.DATA_LEN < 0x00 || gRemsPro.Body.A_ID.DATA_LEN > 0xFF) {
                                                            gRemsPro.Body.A_ID.DATA_LEN = 0;
                                                            continue;
                                                        }


                                                        gRemsPro.Body.A_ID.LayerCnt = buffer[bodyindex++];
                                                        gRemsPro.Body.A_ID.CarreirCnt = buffer[bodyindex++];
                                                        mCarreirCnt = 0;
                                                        gRemsPro.Body.A_ID.Flag = buffer[bodyindex++];
                                                    } else {
                                                        gRemsPro.Body.A_ID.A_ID = 0;
                                                        continue;
                                                    }

                                                    if (gRemsPro.Body.A_ID.DATA_LEN * gRemsPro.Body.A_ID.LayerCnt * gRemsPro.Body.A_ID.CarreirCnt + Macro.A_ID_HEAD_LEN != gRemsPro.Body.A_ID.LEN) {
                                                        gRemsPro.Body.A_ID.DATA_LEN = 0;
                                                        gRemsPro.Body.A_ID.LEN = 0;
                                                        continue;
                                                    }
                                                    bodyindex = Parse_AID_0x50(buffer, bodyindex, gRemsPro.Body.A_ID.A_ID);
                                                    bodyindex = Parse_AID_0x52(buffer, bodyindex, gRemsPro.Body.A_ID.A_ID);
                                                    bodyindex = Parse_AID_0x53(buffer, bodyindex, gRemsPro.Body.A_ID.A_ID);
                                                    bodyindex = Parse_AID_0x55(buffer, bodyindex, gRemsPro.Body.A_ID.A_ID);
                                                    bodyindex = Parse_AID_0x56(buffer, bodyindex, gRemsPro.Body.A_ID.A_ID);
                                                    bodyindex = Parse_AID_0xB0(buffer, bodyindex, gRemsPro.Body.A_ID.A_ID);
                                                    bodyindex--;

                                                }
                                                buffer = new byte[8096];
                                                bytes = 0;
                                                gRemsPro.ToTLenPacket = 0;
                                                gRemsPro.CunrLenPacket = 0;
                                                gRemsPro.ToTBodyLen = 0;
                                                gRemsPro.Body.A_ID.LEN = 0;
                                                gRemsPro.Body.LEN = 0;
                                                gRemsPro.Body.SUB_ID = 0;
                                                mBluetoothHandler.sendMessage(msg);
                                                break;
                                            }
                                        }
                                        break;
                                    } else {
                                        Count = 0;
                                        ParseIndex = 0;
                                        buffer = new byte[8096];
                                        bytes = 0;
                                        break;
                                    }
                                }
                            }
                            mTransceiver = false;
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

            public int Parse_AID_0x50 ( byte[] data, int localindex, int AID){

                switch (AID) {
                    case AID_ID:
                        A_id_data.RU_ID = (data[localindex++] << 8 & 0xFF00);
                        A_id_data.RU_ID = data[localindex++];
                        break;
                    case AID_CASCADE_ID:
                        A_id_data.CASCADE_ID = data[localindex++];
                        break;
                    case AID_LAYER_USE_INFO:
                        A_id_data.LAYER_USE_INFO = data[localindex++];
                        break;
                    case AID_PCI:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.PCI_100M = (short) ((data[localindex++] << 8) & 0xFF00);
                            A_id_data.PCI_100M = (short) (data[localindex++]);
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.PCI_a = (short) ((data[localindex++] << 8) & 0xFF00);
                            A_id_data.PCI_a = (short) (data[localindex++]);
                        }
                        break;
                    case AID_CELL_ID:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.Cell_id_100M = data[localindex++];
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.Cell_id_a = data[localindex++];
                        }
                        break;
                    case AID_LS_ID:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.Ls_id_100M = data[localindex++];
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.Ls_id_a = data[localindex++];
                        }
                        break;
                    case AID_GNB_ID:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.Gnb_id_100M = (data[localindex++] << 32 & 0xFF000000);
                            A_id_data.Gnb_id_100M = (data[localindex++] << 16 & 0xFF0000);
                            A_id_data.Gnb_id_100M = (data[localindex++] << 8 & 0xFF00);
                            A_id_data.Gnb_id_100M = data[localindex++];
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.Gnb_id_a = (data[localindex++] << 32 & 0xFF000000);
                            A_id_data.Gnb_id_a = (data[localindex++] << 16 & 0xFF0000);
                            A_id_data.Gnb_id_a = (data[localindex++] << 8 & 0xFF00);
                            A_id_data.Gnb_id_a = data[localindex++];
                        }
                        break;
                    case AID_PORT_NO:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.PortNo_100M = data[localindex++];
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.PortNo_a = data[localindex++];
                        }
                        break;
                    case AID_CH_BW:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.ChBw_100M = data[localindex++];
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.ChBw_a = data[localindex++];
                        }
                        break;
                    case AID_LS_MANU:
                        A_id_data.LS_MANU = data[localindex++];
                        break;
                    case AID_CENTER_FRQ:
                        if (gRemsPro.Body.A_ID.CarreirCnt - Macro.A_ID_CARRIER_FACTOR == ++mCarreirCnt) {
                            A_id_data.CenFreq_100M = 0;
                            A_id_data.CenFreq_100M += (data[localindex++] << 32 & 0xFF000000);
                            A_id_data.CenFreq_100M += (data[localindex++] << 16 & 0xFF0000);
                            A_id_data.CenFreq_100M += (data[localindex++] << 8 & 0xFF00);
                            A_id_data.CenFreq_100M += data[localindex++];
                            A_id_data.CenFreq_100M = A_id_data.CenFreq_100M/1000;
                            Global.SetCenFreq_100M(A_id_data.CenFreq_100M);
                        }
                        if (gRemsPro.Body.A_ID.CarreirCnt == ++mCarreirCnt) {
                            A_id_data.CenFreq_a = 0;
                            A_id_data.CenFreq_a += (data[localindex++] << 32 & 0xFF000000);
                            A_id_data.CenFreq_a += (data[localindex++] << 16 & 0xFF0000);
                            A_id_data.CenFreq_a += (data[localindex++] << 8 & 0xFF00);
                            A_id_data.CenFreq_a += data[localindex++];
                            A_id_data.CenFreq_a = A_id_data.CenFreq_a/1000;
                            Global.SetCenFreq_a(A_id_data.CenFreq_a);
                        }
                        break;
                    case AID_RU_INIT_ST:
                        A_id_data.RU_INIT_STATE = data[localindex++];
                        break;

                    case AID_COND_WATCH_DEVICE_CON:
                        break;

                    case AID_RU_TEMP:
                        A_id_data.RU_TEMP = 0;
                        A_id_data.RU_TEMP += ((data[localindex++] << 8) & 0xFF00);
                        A_id_data.RU_TEMP += (data[localindex++] & 0xFF);
                        A_id_data.RU_TEMP = A_id_data.RU_TEMP/10;
                        break;

                    case AID_RU_TEMP_MAX:
                        A_id_data.RU_TEMP_MAX = 0;
                        A_id_data.RU_TEMP_MAX += ((data[localindex++] << 8) & 0xFF00);
                        A_id_data.RU_TEMP_MAX += (data[localindex++] & 0xFF);
                        A_id_data.RU_TEMP_MAX = A_id_data.RU_TEMP_MAX/10;
                        break;

                    case AID_FAN1:
                        break;

                    case AID_DC:
                        break;

                    case AID_BATTERY_LOW:
                        break;

                    case AID_DL_TSSI:
                        break;

                    case AID_DL_TSSI_MAX_ONE:
                        break;

                    case AID_DL_TSSI_MAX_TWO:
                        break;

                    case AID_DL_OUTPUT:
                        break;

                    case AID_DL_OUTPUT_MAX:
                        break;

                    case AID_DL_OUTPUT_MIN:
                        break;

                    case AID_DL_ATTN:
                        break;

                    case AID_DL_TSSI_DEC:
                        break;

                    case AID_DL_OUTPUT_CAL:
                        break;
                    case AID_OVR_PWR:
                        break;
                    case AID_SHUTDOWN_DL:
                        break;
                    case AID_PAU_ONOFF:
                        break;
                    case AID_DL_DISABLED:
                        break;

                    case AID_SW_VER:
                        break;
                    default:
                }
                return localindex;
            }
            public int Parse_AID_0x52 ( byte[] data, int localindex, int AID){
                switch (AID) {
                    case AID_UL_RSSI:
                        break;
                    case AID_UL_RSSI_MAX:
                        break;
                    case AID_UL_ATTN:
                        break;
                    case AID_UL_GAIN:
                        break;
                    case AID_UL_GAIN_BACKOFF:
                        break;
                    case AID_UL_GAIN_BALANCE:
                        break;
                    case AID_UL_OVR_IN:
                        break;
                    case AID_UL_GAIN_LO:
                        break;
                    case AID_UL_LNA_ONOFF:
                        break;
                }
                return localindex;
            }
            public int Parse_AID_0x53 ( byte[] data, int localindex, int AID){
                switch (AID) {
                    case AID_CLK:
                        break;
                    case AID_CPRI:
                        break;
                    case AID_SFP_LOS:
                        break;
                    case AID_CPRI_LOF:
                        break;
                    case AID_SFP_LD:
                        break;
                    case AID_CPRI_LD_PWR:
                        break;
                    case AID_CPRI_PD_PWR:
                        break;
                    case AID_CPRI_BIP_ERR_CNT:
                        break;
                    case AID_CPRI_SFP_VCC:
                        break;
                    case AID_CPRI_LD_BIAS:
                        break;
                    case AID_CPRI_WAVE:
                        break;
                    case AID_CPRI_BITRATE:
                        break;
                    case AID_CPRI_SFP_TMP:
                        break;
                    case AID_SFP_TAKEN:
                        break;
                    case AID_DL_HARD_CLIPING:
                        break;
                    default:
                        break;
                }
                return localindex;
            }
            public int Parse_AID_0x54 ( byte[] data, int localindex, int AID){
                switch (AID) {
                    case AID_TOFFSET:
                        break;
                    case AID_TB_DELAY_DL:
                        break;
                    case AID_TB_DELAY_UL:
                        break;
                    case AID_T2A_DL_DELAY:
                        break;
                    case AD_TA3_UL_DELAY:
                        break;
                    case AID_OPTIC_DELAY:
                        break;
                    case AID_DLUL_VERIF:
                        break;
                    case AID_CPRI_BIP_ERR_CNT:
                        break;
                    case AID_CPRI_SFP_VCC:
                        break;
                    case AID_CPRI_LD_BIAS:
                        break;
                    case AID_CPRI_WAVE:
                        break;
                    case AID_CPRI_BITRATE:
                        break;
                    case AID_CPRI_SFP_TMP:
                        break;
                    case AID_SFP_TAKEN:
                        break;
                    case AID_DL_HARD_CLIPING:
                        break;
                    default:
                        break;
                }
                return localindex;
            }
            public int Parse_AID_0x55 ( byte[] data, int localindex, int AID){
                switch (AID) {
                    case AID_SW_VER:
                        A_id_data.SW_VER[0] = data[localindex++];
                        A_id_data.SW_VER[1] = data[localindex++];
                        A_id_data.SW_VER[2] = data[localindex++];
                        break;
                    case AID_MANU:
                        A_id_data.MANU = data[localindex++];
                        break;
                    case AID_TYPE:
                        A_id_data.RuType = data[localindex++];
                        break;
                    default:
                        break;
                }
                return localindex;
            }
            public int Parse_AID_0x56 ( byte[] data, int localindex, int AID){
                switch (AID) {
                    case AID_DOWNLOAD_STAT:
                        A_id_data.DL_STATE[0] = data[localindex++];
                        A_id_data.DL_STATE[1] = data[localindex++];
                        break;
                    case AID_ID_MISMATCH:
                        A_id_data.MISMATCH_ID = data[localindex++];
                        break;
                    default:
                        break;
                }
                return localindex;
            }
            public int Parse_AID_0xB0 ( byte[] data, int localindex, int AID){
                switch (AID) {
                    case AID_CONF_ENV_TREE:
                        A_id_data.TREE_INFO[0] = data[localindex++];
                        A_id_data.TREE_INFO[Macro.RU_ONE_STATE] = data[localindex++];
                        A_id_data.TREE_INFO[Macro.RU_TWO_STATE] = data[localindex++];
                        A_id_data.TREE_INFO[Macro.RU_THREE_STATE] = data[localindex++];
                        A_id_data.TREE_INFO[Macro.RU_FOUR_STATE] = data[localindex++];
                        A_id_data.TREE_INFO[Macro.RU_CON_GUI] = data[localindex++];
                        break;
                    default:
                        break;
                }
                return localindex;
            }

            int Generate_CRC ( byte[] buf_ptr, int index, int len)
            {
                int data, crc_16;
                len *= 8;
                int square = 0;
                int Crc16_Table_Index = 0;
                for (crc_16 = CRC_16_SEED; len >= 8; len -= 8, index++) {
                    square = 0;
                    square = (buf_ptr[index] & 0xFF);
                    Crc16_Table_Index = (int) Math.pow((crc_16 >> 8 & 0xFF), square);
                    crc_16 = (int) Math.pow(Crc16_Table[Crc16_Table_Index], ((crc_16 << 8)));
                }

                if (len != 0) {
                    data = ((int) (buf_ptr[index])) << (8);
                    while (len-- != 0) {
                        if (((crc_16 ^ data) & ((int) 1 << 15)) != 0) {
                            crc_16 <<= 1;
                            crc_16 ^= CRC_16_POLYNOMIAL;
                        } else crc_16 <<= 1;
                        data <<= 1;
                    }
                }
                return (crc_16);
            }

            public void write (Byte bytes){
                //byte[] bytes = str.getBytes();
                try {
                    mmOutStream.write(bytes);
                } catch (IOException e) {
//                    Toast.makeText(getApplicationContext(), "데이터 전송 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
                    IsConnectedBT = false;
                    ReConnectDevice();
                }
            }
            public void cancel () {
                try {
                    mmSocket.close();
                } catch (IOException e) {
                    Toast.makeText(getApplicationContext(), "소켓 해제 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
                }
            }
        }

    public class BtRecevier extends BroadcastReceiver {
        public void onReceive(Context context, Intent intent){
            BluetoothAdapter BtAdapter;
            BluetoothDevice BtDevice;

            String Action = intent.getAction();
            BtAdapter = BluetoothAdapter.getDefaultAdapter();

        }
    }

    class gRemmsPacket{
        byte Packet[];
        int index;
    }

    class gRemsProtocol{
        byte STX[] = new byte[4];
        short SeqNum;
        byte ToTLenPacket;
        byte CunrLenPacket;
        short ToTBodyLen;
        byte InfoCheckSum;
        BodyProtocol Body = new BodyProtocol();
        byte ETX;
    }

    class BodyProtocol{
        int SUB_ID;
        byte CMD = 0;
        byte RCODE;
        int LEN = 0;
        A_ID_Protocol A_ID = new A_ID_Protocol();
        short CRC;
    }

    class A_ID_Protocol{
        int A_ID;
        short LEN;
        byte DataType;
        byte DataStep;
        byte Reserved;
        byte DATA_LEN = 1;
        byte LayerCnt;
        byte CarreirCnt;
        byte Flag;
        A_ID_Payload A_id_Payload = new A_ID_Payload();
    }

    class A_ID_Payload{
        byte LayerCnt;
        byte CarrierCnt;
        byte Roundup;
    }

    class gRems_Macro {
        final byte STX = 0x16;
        final byte ETX = 0x03;
        final byte HEAD_LEN = 0x04;
        int A_ID_INDEX = 0x00;
        final byte A_ID_HEAD_LEN = 0x07;
        final byte A_ID_CARRIER_FACTOR = 0x01;
        final int RU_ONE_STATE = 0x01;
        final int RU_TWO_STATE = 0x02;
        final int RU_THREE_STATE = 0x03;
        final int RU_FOUR_STATE = 0x04;
        final int RU_CON_GUI = 0x05;
    }
}