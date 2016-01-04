package com.example.android.jukebox;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.client.DefaultHttpRequestRetryHandler;
import org.apache.http.message.BasicNameValuePair;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;


public class ChooseSong extends Activity {

    final static String ip_message = "ip";
    final static String port_message = "port";
    EditText mEdit;
    Button mButton;
    ImageView imageView;
    TextView textView2;
    Chronometer chronometer;
    ImageView ipView;
    String ip_return;
    String port_return;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION,
                WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS,
                WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);

        Intent intent = getIntent();

        setContentView(R.layout.activity_choose);

        final String ip = intent.getStringExtra(ChangeIP.ip_message);
        ip_return = ip;
        final String port = intent.getStringExtra(ChangeIP.port_message);
        port_return = port;
        mButton = (Button) findViewById(R.id.enter_button);
        mEdit = (EditText) findViewById(R.id.editText);
        imageView = (ImageView) findViewById(R.id.imageView);
        textView2 = (TextView) findViewById(R.id.textView2);
        chronometer = (Chronometer) findViewById(R.id.chronometer);
        ipView = (ImageView) findViewById(R.id.ipView);

        textView2.setVisibility(View.INVISIBLE);
        chronometer.setVisibility(View.INVISIBLE);


        imageView.setImageResource(R.drawable.title_image);

        UiChangeListener();




        mButton.setOnClickListener(
                new View.OnClickListener() {
                    public void onClick(View view) {
                        textView2.setText("Downloading...");
                        String givenKeyword = mEdit.getText().toString();
                        InputMethodManager imm = (InputMethodManager) getSystemService(
                                Context.INPUT_METHOD_SERVICE);
                        imm.hideSoftInputFromWindow(mEdit.getWindowToken(), 0);
                        if(ip == null | port == null){
                            Toast.makeText(getApplicationContext(), "Please enter an IP address and Port number!",
                                    Toast.LENGTH_LONG).show();

                        }
                        else if (givenKeyword.length() < 1) {
                            Toast.makeText(getApplicationContext(), "Please enter a keyword!",
                                    Toast.LENGTH_LONG).show();
                        } else {
                            chronometer.setVisibility(View.VISIBLE);
                            ipView.setVisibility(View.INVISIBLE);
                            chronometer.setBase(SystemClock.elapsedRealtime());
                            chronometer.start();
                            Log.v("EditText", givenKeyword);
                            new MyAsyncTask().execute(givenKeyword, ip, port);
                            textView2.setVisibility(View.VISIBLE);
                        }
                    }
                });

    }
    public void quitMenu (View view){
        Intent intent = new Intent(ChooseSong.this, MenuActivity.class);
        startActivity(intent);
    }
    public void openIP (View view){
        Intent intent2 = new Intent(ChooseSong.this, ChangeIP.class);
        intent2.putExtra(ip_message, ip_return);
        intent2.putExtra(port_message, port_return);
        startActivity(intent2);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        View decorView = getWindow().getDecorView();
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            decorView.setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
        }
    }

    public void UiChangeListener() {
        final View decorView = getWindow().getDecorView();
        decorView.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
            @Override
            public void onSystemUiVisibilityChange(int visibility) {
                if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
                    decorView.setSystemUiVisibility(
                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
                }
            }
        });
    }

    private class MyAsyncTask extends AsyncTask<String, Integer, String> {

        String header;

        @Override
        protected String doInBackground(String... params) {
            // Create a new HttpClient and Post Header
            DefaultHttpClient httpclient = new DefaultHttpClient();
            httpclient.setHttpRequestRetryHandler(new DefaultHttpRequestRetryHandler(
                    0, false));
            HttpPost httppost = new HttpPost("http://"+params[1]+":"+params[2]);
            try {
                // Add your data
                List<NameValuePair> nameValuePairs = new ArrayList<NameValuePair>(1);
                nameValuePairs.add(new BasicNameValuePair("keywords", params[0]));
                httppost.setEntity(new UrlEncodedFormEntity(nameValuePairs));

                // Execute HTTP Post Request
                HttpResponse response = httpclient.execute(httppost);
                header = response.getFirstHeader("response").toString();



            } catch (ClientProtocolException e) {
                // TODO Auto-generated catch block
            } catch (IOException e) {
                // TODO Auto-generated catch block
            }

            return null;
        }

        protected void onPostExecute(String result) {
                textView2.setText("Downloaded!");
                chronometer.stop();
                long elapsedTime = (SystemClock.elapsedRealtime() - chronometer.getBase()) / 1000;
                Toast.makeText(getApplicationContext(), "Your song was downloaded in"+" " + elapsedTime + " "+"seconds!",
                        Toast.LENGTH_LONG).show();
                ipView.setVisibility(View.VISIBLE);



        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}

