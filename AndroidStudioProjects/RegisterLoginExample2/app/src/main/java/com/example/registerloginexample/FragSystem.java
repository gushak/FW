package com.example.registerloginexample;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.SystemClock;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class FragSystem extends Fragment {

    private HandlerThread handlerThread;
    private Handler mSystemHandler;

    private View view;
    private Context context;
    //Tv
    TextView mTvRuType;

    TextView mTvSwVer;
    TextView mTvManu;
    TextView mTvRuTemp;
    TextView mTvRuTempMax;
    TextView mTvDLState;
    TextView mTvRuInitState;
    TextView mTvLsManu;
    TextView mTvRuId;
    TextView mTvCascadeID;
    TextView mTvMismatchID;

    TextView mTvPci_100M;
    TextView mTvCellId_100M;
    TextView mTvGnbId_100M;
    TextView mTvLSId_100M;
    TextView mTvPortNo_100M;
    TextView mTvChBw_100M;
    TextView mTvCenFreq_100M;

    TextView mTvPci_a;
    TextView mTvCellId_a;
    TextView mTvGnbId_a;
    TextView mTvLSId_a;
    TextView mTvPortNo_a;
    TextView mTvChBw_a;
    TextView mTvCenFreq_a;

    //Button
    Button mBtnReset;


    public static FragSystem newInstance(){
        FragSystem fragSystem = new FragSystem();
        return fragSystem;
    }

    @Override
    public void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        init();
        start();
    }

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        context = container.getContext();
        view = inflater.inflate(R.layout.frag_system,container,false);
        ViewGroup root = (ViewGroup) inflater.inflate(R.layout.frag_system, null);

        //        mTvRuType = (TextView)view.findViewById(R.id.tvRuType);
        mTvSwVer = (TextView)root.findViewById(R.id.tvSwVer);
        mTvManu = (TextView)root.findViewById(R.id.tvManu);
        mTvRuTemp = (TextView)root.findViewById(R.id.tvRuTemp);
        mTvRuTempMax = (TextView)root.findViewById(R.id.tvRuTempMax);
        mTvDLState = (TextView)root.findViewById(R.id.tvDLState);
        mTvRuInitState = (TextView)root.findViewById(R.id.tvRuInitState);

        mTvLsManu = (TextView)root.findViewById(R.id.tvLsManu);
        mTvRuId = (TextView)view.findViewById(R.id.tvRuId);
        mTvCascadeID = (TextView)root.findViewById(R.id.tvCascadeId);
        mTvMismatchID = (TextView)root.findViewById(R.id.tvMismatchId);

        mTvRuId.setText("hello");
//        mTvPci_100M = (TextView)getView().findViewById(R.id.tvPci_100M);
//        mTvCellId_100M = (TextView)getView().findViewById(R.id.tvCellId_100M);
//        mTvGnbId_100M = (TextView)getView().findViewById(R.id.tvGnbId_100M);
//        mTvLSId_100M = (TextView)getView().findViewById(R.id.tvLsId_100M);
//        mTvPortNo_100M = (TextView)getView().findViewById(R.id.tvPortNum_100M);
//        mTvChBw_100M = (TextView)getView().findViewById(R.id.tvChBand_100M);
//        mTvCenFreq_100M = (TextView)getView().findViewById(R.id.tvCenFerq_100M);
//        mTvPci_a = (TextView)getView().findViewById(R.id.tvPci_a);
//        mTvCellId_a = (TextView)getView().findViewById(R.id.tvCellId_a);
//        mTvGnbId_a = (TextView)getView().findViewById(R.id.tvGnbId_a);
//        mTvLSId_a = (TextView)getView().findViewById(R.id.tvLsId_a);
//        mTvPortNo_a = (TextView)getView().findViewById(R.id.tvPortNum_a);
//        mTvChBw_a = (TextView)getView().findViewById(R.id.tvChBand_a);
//        mTvCenFreq_a = (TextView)getView().findViewById(R.id.tvCenFerq_a);
//
//        mTvRuTemp = (TextView)getView().findViewById(R.id.tvRuTemp);
//        mTvRuTempMax = (TextView)getView().findViewById(R.id.tvRuTempMax);
//        mTvDLState = (TextView)getView().findViewById(R.id.tvDLState);
//
//        mBtnReset = (Button)getView().findViewById(R.id.btnReset);
//
//        mBtnReset.setOnClickListener(new Button.OnClickListener() {
//            @Override
//            public void onClick(View view){
//                Global.SetTxReset(true);
//                Toast.makeText(context, "RCU Reset", Toast.LENGTH_LONG).show();
//            }
//        });

        return view;
    }


    public void init(){
        handlerThread = new HandlerThread("System");
        handlerThread.start();
    }

    public void start(){
        new Handler(handlerThread.getLooper()).post(new Runnable() {

            Intent data = getActivity().getIntent();

            @Override
            public void run() {
                A_ID_DATA A_id_data = data.getParcelableExtra("Intent_Aid");

                while(true) {
                    SystemClock.sleep(100);
                    System.out.println("메인 동작중 ");

//                    mTvRuId.setText("hello");
//                    mTvRuId.setText(Integer.toString(A_id_data.getRU_ID()));
//                    mTvSwVer.setText(Integer.toString(A_id_data.SW_VER[0]) + "." + Integer.toString(A_id_data.SW_VER[1]) +"."+ Integer.toString(A_id_data.SW_VER[2]));
//
//                    if(A_id_data.MANU == 0x26) {
//                        mTvManu.setText("ANTS");
//                    }
//
//                    mTvRuTemp.setText(Float.toString(A_id_data.RU_TEMP));
//                    mTvRuTempMax.setText(Float.toString(A_id_data.RU_TEMP_MAX));
//
//                    if((A_id_data.DL_STATE[0]) == 0x00)
//                    {
//                        mTvDLState.setText("Idle (전원 Reset등으로 초기화)");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x01)
//                    {
//                        mTvDLState.setText("D/L 완료(Reset등으로 초기화)");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x02)
//                    {
//                        mTvDLState.setText("D/L중");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x03)
//                    {
//                        mTvDLState.setText("D/L 실패 (Timeout)");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x04)
//                    {
//                        mTvDLState.setText("Reset 준비중");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x05)
//                    {
//                        mTvDLState.setText("압축 해제 중");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x06)
//                    {
//                        mTvDLState.setText("압축 파일 유효성 검사 중");
//                    }
//                    else if((A_id_data.DL_STATE[0]) == 0x07)
//                    {
//                        mTvDLState.setText("압축 해제 후 내부 D/L 중");
//                    }
//
//                    //Ru Init Status
//                    if(A_id_data.RU_INIT_STATE == 0x00)
//                    {
//                        mTvRuInitState.setText("(1/6) System Initialization");
//                    }
//                    else if(A_id_data.RU_INIT_STATE == 0x01){
//                        mTvRuInitState.setText("(2/6) RE ID Acquisition Notification ACK waiting");
//                    }
//                    else if(A_id_data.RU_INIT_STATE == 0x02){
//                        mTvRuInitState.setText("(3/6) Initialization Notification ACk waiting");
//                    }
//                    else if(A_id_data.RU_INIT_STATE == 0x03){
//                        mTvRuInitState.setText("(4/6) CPRI Status Request waiting");
//                    }
//                    else if(A_id_data.RU_INIT_STATE == 0x04){
//                        mTvRuInitState.setText("(5/6) Delay Setup Request waiting");
//                    }
//                    else if(A_id_data.RU_INIT_STATE == 0x05){
//                        mTvRuInitState.setText("(6/6) Initialization Complete");
//                    }
//
//                    //LS Manu
//                    if(A_id_data.LS_MANU == 0xEA)
//                        mTvLsManu.setText("SS");
//                    else if(A_id_data.LS_MANU == 0xEC)
//                        mTvLsManu.setText("ELG");
//                    else if(A_id_data.LS_MANU == 0xEE)
//                        mTvLsManu.setText("NKA");
//                    else
//                        mTvLsManu.setText("Unknown");
//
//                    mTvCascadeID.setText(Integer.toString(A_id_data.CASCADE_ID));
//
//                    mTvPci_100M.setText(Integer.toString(A_id_data.PCI_100M));
//                    mTvPci_a.setText(Integer.toString(A_id_data.PCI_a));
//
//                    mTvCellId_100M.setText(Integer.toString(A_id_data.Cell_id_100M));
//                    mTvCellId_a.setText(Integer.toString(A_id_data.Cell_id_a));
//
//                    mTvGnbId_100M.setText(Integer.toString(A_id_data.Gnb_id_100M));
//                    mTvGnbId_a.setText(Integer.toString(A_id_data.Gnb_id_a));
//
//                    mTvLSId_100M.setText(Integer.toString(A_id_data.Ls_id_100M));
//                    mTvLSId_a.setText(Integer.toString(A_id_data.Ls_id_a));
//
//                    mTvPortNo_100M.setText(Integer.toString(A_id_data.PortNo_100M));
//                    mTvPortNo_a.setText(Integer.toString(A_id_data.PortNo_a));
                    //Type
                    //mTvRuType.setText();

//                    if(A_id_data.ChBw_100M == 0x00){
//                        mTvChBw_100M.setText("미사용");
//                    }
//                    else if(A_id_data.ChBw_100M == 0x01){
//                        mTvChBw_100M.setText("10Mhz");
//                    }
//                    else if(A_id_data.ChBw_100M == 0x02){
//                        mTvChBw_100M.setText("20Mhz");
//                    }
//                    else if(A_id_data.ChBw_100M == 0x03) {
//                        mTvChBw_100M.setText("40Mhz");
//                    }
//                    else if(A_id_data.ChBw_100M == 0x04){
//                        mTvChBw_100M.setText("60Mhz");
//                    }
//                    else if(A_id_data.ChBw_100M == 0x05){
//                        mTvChBw_100M.setText("80Mhz");
//                    }
//                    else if(A_id_data.ChBw_100M == 0x06){
//                        mTvChBw_100M.setText("100Mhz");
//                    }
//
//                    if(A_id_data.ChBw_a == 0x00){
//                        mTvChBw_a.setText("미사용");
//                    }
//                    else if(A_id_data.ChBw_a == 0x01){
//                        mTvChBw_a.setText("10Mhz");
//                    }
//                    else if(A_id_data.ChBw_a == 0x02){
//                        mTvChBw_a.setText("20Mhz");
//                    }
//                    else if(A_id_data.ChBw_a == 0x03) {
//                        mTvChBw_a.setText("40Mhz");
//                    }
//                    else if(A_id_data.ChBw_a == 0x04){
//                        mTvChBw_a.setText("60Mhz");
//                    }
//                    else if(A_id_data.ChBw_a == 0x05){
//                        mTvChBw_a.setText("80Mhz");
//                    }
//                    else if(A_id_data.ChBw_a == 0x06){
//                        mTvChBw_a.setText("100Mhz");
//                    }
//
//                    mTvCenFreq_100M.setText(Float.toString(Global.GetCenFreq_100M()));
//                    mTvCenFreq_a.setText(Float.toString(Global.GetCenFreq_a()));
                }
//                Log.e("RU_ID",getResources().getString(A_id_data.getRU_ID()));
            }
        });
    }
}
