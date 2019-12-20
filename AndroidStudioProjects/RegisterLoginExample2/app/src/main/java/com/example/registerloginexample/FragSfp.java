package com.example.registerloginexample;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

public class FragSfp extends Fragment {
    private View view;

    public static FragSfp newInstance(){
        FragSfp fragSfp = new FragSfp();
        return fragSfp;
    }
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.frag_sfp,container,false);

        return view;
    }
}
