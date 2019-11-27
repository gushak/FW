
package com.example.registerloginexample;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    TextView mTvBluetoothStatus;
    TextView mTvReceiveData;
    TextView mTvSendData;
    Button mBtnBluetoothOn;
    Button mBtnBluetoothOff;
    Button mBtnConnect;
    Button mBtnSendData;

    BluetoothAdapter mBluetoothAdapter;
    Set<BluetoothDevice> mPairedDevices;
    List<String> mListPairedDevices;

    Handler mBluetoothHandler;
    ConnectedBluetoothThread mThreadConnectedBluetooth;
    BluetoothDevice mBluetoothDevice;
    BluetoothSocket mBluetoothSocket;

    final static int BT_REQUEST_ENABLE = 1;
    final static int BT_MESSAGE_READ = 2;
    final static int BT_CONNECTING_STATUS = 3;
    final static UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    gRemsProtocol gRemsPro = new gRemsProtocol();
    gRemmsPacket gRemsPack = new gRemmsPacket();
    gRems_Macro Macro;
    A_ID a_id;
    static byte[] RxByte = new byte[8096];
    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTvBluetoothStatus = (TextView) findViewById(R.id.tvBluetoothStatus);
        mTvReceiveData = (TextView) findViewById(R.id.tvReceiveData);
        mTvSendData = (EditText) findViewById(R.id.tvSendData);
        mBtnBluetoothOn = (Button) findViewById(R.id.btnBluetoothOn);
        mBtnBluetoothOff = (Button) findViewById(R.id.btnBluetoothOff);
        mBtnConnect = (Button) findViewById(R.id.btnConnect);
        mBtnSendData = (Button) findViewById(R.id.btnSendData);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        mBtnBluetoothOn.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
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
                listPairedDevices();
            }
        });
        mBtnSendData.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mThreadConnectedBluetooth != null) {
                    //mThreadConnectedBluetooth.write(mTvSendData.getText().toString());
                    final byte tx[] = new byte[]{0x16, 0x16, 0x16, 0x16, 0x00, 0x0c, 0x01, 0x01, 0x00, 0x09, 0x17, (byte) 0xff, (byte) 0xff, (byte) 0xff, 0x00, 0x50, 0x00, 0x00, (byte) 0xf5, (byte) 0xaf, 0x03};

                    for (int i = 0; i < tx.length; i++)
                        mThreadConnectedBluetooth.write(tx[i]);

                    mTvSendData.setText("");
                }
            }
        });

        mBluetoothHandler = new Handler() {
            public synchronized void handleMessage(android.os.Message msg) {
                if (msg.what == BT_MESSAGE_READ) {
                    RxByte = (byte[]) msg.obj;
                    int Count = 0;

                    if (RxByte != null) {
                    for (int ParseIndex = 0; ParseIndex < RxByte.length; ParseIndex++) {

                        if (RxByte[ParseIndex] == 0x16 && Count == 0) {
                            Count++;
                        } else if (RxByte[ParseIndex] == 0x16 && Count == 1) {
                            Count++;
                        } else if (RxByte[ParseIndex] == 0x16 && Count == 2) {
                            Count++;
                        } else if (RxByte[ParseIndex] == 0x16 && Count == 3) {
                            Count++;
                        } else if (Count > 4) {
                            gRemsPro.SeqNum += ((RxByte[ParseIndex++] << 8) & 0xFF00);
                            gRemsPro.SeqNum += RxByte[ParseIndex++];
                            gRemsPro.TotLen = RxByte[ParseIndex++];
                            gRemsPro.CurLen = RxByte[ParseIndex++];
                            if (gRemsPro.TotLen >= gRemsPro.CurLen) {
                                gRemsPro.BodyLen += ((RxByte[ParseIndex++] << 8) & 0xFF00);
                                gRemsPro.BodyLen += RxByte[ParseIndex++];
                                gRemsPro.InfoCheckSum = RxByte[ParseIndex++];

                                byte CalCheckSum = 0;

                                for (int crcindex = 0; crcindex < 6; crcindex++) {
                                    CalCheckSum += RxByte[crcindex];
                                }
                                if (CalCheckSum == gRemsPro.InfoCheckSum) {
                                        gRemsPro.Body.SUB_ID += RxByte[ParseIndex++];
                                        gRemsPro.Body.SUB_ID += RxByte[ParseIndex++];
                                        gRemsPro.Body.SUB_ID += RxByte[ParseIndex++];

                                        gRemsPro.Body.CMD = RxByte[ParseIndex++];
                                        gRemsPro.Body.RCODE = RxByte[ParseIndex++];
                                        gRemsPro.Body.LEN += (0xFF00 & RxByte[ParseIndex++] << 8);
                                        gRemsPro.Body.LEN += RxByte[ParseIndex++];
                                    }
                                }
                                break;
                            }
                        }
                    }
                 }
             }
        };
}

    void bluetoothOn() {
        if(mBluetoothAdapter == null) {
            Toast.makeText(getApplicationContext(), "블루투스를 지원하지 않는 기기입니다.", Toast.LENGTH_LONG).show();
        }
        else {
            if (mBluetoothAdapter.isEnabled()) {
                Toast.makeText(getApplicationContext(), "블루투스가 이미 활성화 되어 있습니다.", Toast.LENGTH_LONG).show();
                mTvBluetoothStatus.setText("활성화");
            }
            else {
                Toast.makeText(getApplicationContext(), "블루투스가 활성화 되어 있지 않습니다.", Toast.LENGTH_LONG).show();
                Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
            }
        }
    }
    void bluetoothOff() {
        if (mBluetoothAdapter.isEnabled()) {
            mBluetoothAdapter.disable();
            Toast.makeText(getApplicationContext(), "블루투스가 비활성화 되었습니다.", Toast.LENGTH_SHORT).show();
            mTvBluetoothStatus.setText("비활성화");
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
            mThreadConnectedBluetooth = new ConnectedBluetoothThread(mBluetoothSocket);
            mThreadConnectedBluetooth.start();
            mBluetoothHandler.obtainMessage(BT_CONNECTING_STATUS, 1, -1).sendToTarget();
        } catch (IOException e) {
            Toast.makeText(getApplicationContext(), "블루투스 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
        }
    }

    private class ConnectedBluetoothThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedBluetoothThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 연결 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }
        public void run() {
            byte[] buffer = new byte[8096];
            int bytes;

            while (true) {
                try {
                    bytes = mmInStream.available();
                    if (bytes != 0) {
//                        SystemClock.sleep(300);
                        bytes = mmInStream.available();
                        bytes = mmInStream.read(buffer, 0, bytes);
//                        mBluetoothHandler.obtainMessage(BT_MESSAGE_READ, bytes, -1, buffer).sendToTarget();
                        Message msg = new Message();
                        msg.what = BT_MESSAGE_READ;
                        msg.obj = buffer;
                        msg.arg1 = bytes;
                        mBluetoothHandler.sendMessage(msg);
                    }
                } catch (IOException e) {
                    break;
                }
            }
        }
        public void write(Byte bytes) {
            //byte[] bytes = str.getBytes();
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "데이터 전송 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Toast.makeText(getApplicationContext(), "소켓 해제 중 오류가 발생했습니다.", Toast.LENGTH_LONG).show();
            }
        }
    }
}

class gRemmsPacket{
    byte Packet[];
    int index;
}

class gRemsProtocol{
    byte STX[] = new byte[4];
    short SeqNum;
    byte TotLen;
    byte CurLen;
    short BodyLen;
    byte InfoCheckSum;
    BodyProtocol Body = new BodyProtocol();
    byte ETX;
}

class BodyProtocol{
    int SUB_ID;
    byte CMD = 0;
    byte RCODE;
    int LEN = 0;
    String SUB_DATA;
    String CRC;
}

class gRems_Macro {
    final byte STX = 0x16;
}

class A_ID extends MainActivity{
    final byte UNUSED_IRCS_0 = 0x00;
    final byte UNUSED_IRCS_1 = 0x01;
    final byte UNUSED_IRCS_2 = 0x02;
    final byte UNUSED_AID_0x0B = 0x0B;
    final byte UNUSED_AID_0x0C = 0x0C;
    final byte UNUSED_AID_0x0D = 0x0D;
    final byte UNUSED_AID_0x0E = 0x0E;
    final byte UNUSED_AID_0x0F = 0x0F;
    final byte UNUSED_AID_0x10 = 0x10;
    final byte UNUSED_AID_0x11 = 0x11;
    final byte UNUSED_AID_0x12 = 0x12;
    final byte UNUSED_AID_0x13 = 0x13;
    final byte UNUSED_AID_0x14 = 0x14;
    final byte UNUSED_AID_0x15 = 0x15;
    final byte UNUSED_AID_0x16 = 0x16;
    final byte UNUSED_AID_0x17 = 0x17;
    final byte UNUSED_AID_0x18 = 0x18;
    final byte UNUSED_AID_0x19 = 0x19;
    final byte UNUSED_AID_0x1A = 0x1A;
    final byte UNUSED_AID_0x1B = 0x1B;
    final byte UNUSED_AID_0x1C = 0x1C;
    final byte UNUSED_AID_0x1D = 0x1D;
    final byte UNUSED_AID_0x1E = 0x1E;
    final byte UNUSED_AID_0x1F = 0x1F;
    final byte UNUSED_AID_0x20 = 0x20;
    final byte UNUSED_AID_0x21 = 0x21;
    final byte UNUSED_AID_0x22 = 0x22;
    final byte UNUSED_AID_0x23 = 0x23;
    final byte UNUSED_AID_0x24 = 0x24;
    final byte UNUSED_AID_0x25 = 0x25;
    final byte UNUSED_AID_0x26 = 0x26;
    final byte UNUSED_AID_0x27 = 0x27;
    final byte UNUSED_AID_0x28 = 0x28;
    final byte UNUSED_AID_0x29 = 0x29;
    final byte UNUSED_AID_0x30 = 0x30;
    final byte UNUSED_AID_0x31 = 0x31;
    final byte UNUSED_AID_0x32 = 0x32;
    final byte UNUSED_AID_0x33 = 0x33;
    final byte UNUSED_AID_0x34 = 0x34;
    final byte UNUSED_AID_0x35 = 0x35;
    final byte UNUSED_AID_0x36 = 0x36;
    final byte UNUSED_AID_0x37 = 0x37;
    final byte UNUSED_AID_0x38 = 0x38;
    final byte UNUSED_AID_0x39 = 0x39;
    final byte UNUSED_AID_0x3A = 0x3A;
    final byte UNUSED_AID_0x3B = 0x3B;
    final byte UNUSED_AID_0x3C = 0x3C;
    final byte UNUSED_AID_0x3D = 0x3D;
    final byte UNUSED_AID_0x3E = 0x3E;
    final byte UNUSED_AID_0x3F = 0x3F;
    final byte UNUSED_AID_0x40 = 0x40;
    final byte UNUSED_AID_0x41 = 0x41;
    final byte UNUSED_AID_0x42 = 0x42;
    final byte UNUSED_AID_0x43 = 0x43;
    final byte UNUSED_AID_0x4D = 0x4D;
    final byte UNUSED_AID_0x4E = 0x4E;
    final byte UNUSED_AID_0x4F = 0x4F;
    final byte UNUSED_AID_0x52 = 0x52;
    final byte UNUSED_AID_0x53 = 0x53;
    final byte UNUSED_AID_0x54 = 0x54;
    final byte UNUSED_AID_0x55 = 0x55;
    final byte UNUSED_AID_0x56 = 0x56;
    final byte UNUSED_AID_0x5C = 0x5C;
    final byte UNUSED_AID_0x5D = 0x5D;
    final byte UNUSED_AID_0x5E = 0x5E;
    final byte UNUSED_AID_0x5F = 0x5F;
    final byte UNUSED_AID_0x64 = 0x64;
    final byte UNUSED_AID_0x65 = 0x65;
    final byte UNUSED_AID_0x66 = 0x66;
    final byte UNUSED_AID_0x67 = 0x67;
    final byte UNUSED_AID_0x6C = 0x6C;
    final byte UNUSED_AID_0x6D = 0x6D;
    final byte UNUSED_AID_0x6E = 0x6E;
    final byte UNUSED_AID_0x6F = 0x6F;
    final byte UNUSED_AID_0x70 = 0x70;
    final byte UNUSED_AID_0x71 = 0x71;
    final byte UNUSED_AID_0x72 = 0x72;
    final byte UNUSED_AID_0x73 = 0x73;
    final byte UNUSED_AID_0x74 = 0x74;
    final byte UNUSED_AID_0x75 = 0x75;
    final byte UNUSED_AID_0x76 = 0x76;
    final byte UNUSED_AID_0x77 = 0x77;
    final byte UNUSED_AID_0x78 = 0x78;
    final byte UNUSED_AID_0x79 = 0x79;
    final byte UNUSED_AID_0x7A = 0x7A;
    final byte UNUSED_AID_0x7B = 0x7B;
    final byte UNUSED_AID_0x7C = 0x7C;
    final byte UNUSED_AID_0x7D = 0x7D;
    final byte UNUSED_AID_0x7E = 0x7E;
    final byte UNUSED_AID_0x7F = 0x7F;
    final byte UNUSED_RANGE_2 = 0x43;
    final byte UNUSED_RANGE_3 = 0x4D;
    final byte UNUSED_RANGE_4 = 0x4F;
    final byte UNUSED_RANGE_5 = 0x5C;
    final byte UNUSED_RANGE_6 = 0x5F;
    final byte UNUSED_RANGE_7 = 0x70;
    final byte UNUSED_RANGE_8 = 0x7F;
    final int UNUSED_RANGE_9 = 0xA0;
    final int UNUSED_RANGE_10 = 0xA3;
    final int UNUSED_RANGE_11 = 0xE0;
    final int UNUSED_RANGE_12 = 0xE8;
    final int UNUSED_REMS_0 = 0xEB;
    final int UNUSED_REMS_1 = 0xED;
}
