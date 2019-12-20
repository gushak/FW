package com.example.registerloginexample;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

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


    Thread Tx_thread;
    Thread Bt_thread;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cascade);

        mTvBluetoothStatus = (TextView) findViewById(R.id.tvBluetoothStatus);
        mTvGuiCon_1 = (TextView) findViewById(R.id.tvGuiCon_1);

        mImgAAU_Black_1 = (ImageView) findViewById(R.id.AAU_Black_1);
        mImgAAU_White_1 = (ImageView) findViewById(R.id.AAU_White_1);
        mImgAAU_Black_2 = (ImageView) findViewById(R.id.AAU_Black_2);
        mImgAAU_White_2 = (ImageView) findViewById(R.id.AAU_White_2);
        mImgAAU_Black_3 = (ImageView) findViewById(R.id.AAU_Black_3);
        mImgAAU_White_3 = (ImageView) findViewById(R.id.AAU_White_3);
        mImgAAU_Black_4 = (ImageView) findViewById(R.id.AAU_Black_4);
        mImgAAU_White_4 = (ImageView) findViewById(R.id.AAU_White_4);


        mImgAAU_Black_1.setOnClickListener(new Button.OnClickListener() {

            @Override
            public void onClick(View view) {
                Toast.makeText(getApplicationContext(), "미 사용", Toast.LENGTH_SHORT).show();
            }
        });

        mImgAAU_White_1.setOnClickListener(new Button.OnClickListener() {

            @Override
            public void onClick(View view) {
                Toast.makeText(getApplicationContext(), "5G GUI에 접속하였습니다.", Toast.LENGTH_SHORT).show();
//                Intent intent = new Intent(CascadeActivity.this, MainActivity.class);
//                intent.putExtra("Intent_Aid", A_id_data);
//                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
//                startActivity(intent);
            }
        });


//This section is reconnect bluetooth
//    final Handler bt_handler = new Handler(){
//        @Override
//        public void handleMessage(@NonNull Message msg) {
//            if (IsConnectedBT == true){
//                if (!mBluetoothAdapter.isEnabled()) {
//                    Intent intentBluetoothEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
//                    startActivityForResult(intentBluetoothEnable, BT_REQUEST_ENABLE);
//                }
//            }
//        }
//    };
    }
}