package com.hepta.vcn;

import android.os.Bundle;
import android.text.Html;
import android.text.method.ScrollingMovementMethod;
import android.widget.TextView;

import androidx.annotation.Nullable;

import java.util.Objects;

public class helpActivity extends BaseActivity{

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_help);
         setTitle(getString(R.string.title_help));
         Objects.requireNonNull(getSupportActionBar()).setDisplayHomeAsUpEnabled(true);

        TextView textView = findViewById(R.id.tv_help);
        String content = Utils.getFromAssets("help.html",getApplicationContext());

        textView.setText(Html.fromHtml(content));//这是显示段落文本的,通过解析html
        textView.setMovementMethod(ScrollingMovementMethod.getInstance());//段落文本的话要加这个


    }
}
