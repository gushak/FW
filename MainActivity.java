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
import android.os.SystemClock;
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
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
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
                        SystemClock.sleep(100);
                        bytes = mmInStream.available();
                        bytes = mmInStream.read(buffer);
                        
                        bufferQueue.offer(new byte[bytes]);
                        System.arraycopy(buffer, 0, bufferQueue.peek(), 0, bytes);

                        Message msg = new Message();
                        msg.what = BT_MESSAGE_READ;
                        msg.obj = bufferQueue.poll();
                        msg.arg1 = bytes;
                        int Count = 0;

                        if (buffer != null && buffer[0] == 0x16) {
                            for (int ParseIndex = 0; ParseIndex < buffer.length; ParseIndex++) {

                                if (buffer[ParseIndex] == 0x16 && Count == 0) {
                                    Count++;
                                } else if (buffer[ParseIndex] == 0x16 && Count == 1) {
                                    Count++;
                                } else if (buffer[ParseIndex] == 0x16 && Count == 2) {
                                    Count++;
                                } else if (buffer[ParseIndex] == 0x16 && Count == 3) {
                                    Count++;
                                } else if (Count >= 4) {
                                    gRemsPro.SeqNum += ((buffer[ParseIndex++] << 8) & 0xFF00);
                                    gRemsPro.SeqNum += buffer[ParseIndex++];
                                    gRemsPro.TotLen = buffer[ParseIndex++];
                                    gRemsPro.CurLen = buffer[ParseIndex++];
                                    if (gRemsPro.TotLen >= gRemsPro.CurLen) {
                                        gRemsPro.BodyLen += ((buffer[ParseIndex++] << 8) & 0xFF00);
                                        gRemsPro.BodyLen += buffer[ParseIndex++];
                                        gRemsPro.InfoCheckSum = buffer[ParseIndex++];

                                        byte CalCheckSum = 0;

                                        for (int crcindex = 0; crcindex < 6; crcindex++) {
                                            CalCheckSum += buffer[crcindex];
                                        }
                                        if (CalCheckSum == gRemsPro.InfoCheckSum) {
                                            gRemsPro.Body.SUB_ID += buffer[ParseIndex++];
                                            gRemsPro.Body.SUB_ID += buffer[ParseIndex++];
                                            gRemsPro.Body.SUB_ID += buffer[ParseIndex++];

                                            gRemsPro.Body.CMD = buffer[ParseIndex++];
                                            gRemsPro.Body.RCODE = buffer[ParseIndex++];
                                            gRemsPro.Body.LEN += (0xFF00 & buffer[ParseIndex++] << 8);
                                            gRemsPro.Body.LEN += buffer[ParseIndex++];
                                        }
                                    }
                                    break;
                                }
                                else
                                    Count = 0;
                                    break;
                            }
                        }
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
