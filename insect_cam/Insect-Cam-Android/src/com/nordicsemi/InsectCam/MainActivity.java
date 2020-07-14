/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.nordicsemi.InsectCam;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.text.DateFormat;
import java.text.DecimalFormat;
import java.util.Arrays;
import java.util.Date;


import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.SystemClock;
import android.support.v4.content.LocalBroadcastManager;
import android.text.Html;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity implements RadioGroup.OnCheckedChangeListener {
    private static final int REQUEST_SELECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int UART_PROFILE_READY = 10;
    public static final String TAG = "image_transfer_main";
    private static final int UART_PROFILE_CONNECTED = 20;
    private static final int UART_PROFILE_DISCONNECTED = 21;
    private static final int STATE_OFF = 10;
    private static final int SETTINGS_ACTIVITY = 100;

    private static final String FONT_LABEL_APP_NORMAL = "<font color='#EE0000'>";
    private static final String FONT_LABEL_APP_ERROR = "<font color='#EE0000'>";
    private static final String FONT_LABEL_PEER_NORMAL = "<font color='#EE0000'>";
    private static final String FONT_LABEL_PEER_ERROR = "<font color='#EE0000'>";
    public enum AppLogFontType {APP_NORMAL, APP_ERROR, PEER_NORMAL, PEER_ERROR};
    private String mLogMessage = "";

    TextView mTextViewLog, mTextViewFileLabel, mTextViewPictureStatus, mTextViewConInt;//, mTextViewMtu;
    Button mBtnTakePicture, mBtnStartStream, mBtnStartAccel, mBtnWriteReg;
    ProgressBar mProgressBarFileStatus;
    ImageView mMainImage;
    Spinner mSpinnerResolution, mSpinnerPhy, mSpinnerAngle, mSpinnerDevice;
    EditText mFilename, mRegAddr, mRegVal;
    CheckBox mJPEG, mDebug, mFlip;

    private int mState = UART_PROFILE_DISCONNECTED;
    private ImageTransferService mService = null;
    private BluetoothDevice mDevice = null;
    private BluetoothAdapter mBtAdapter = null;
    private Button btnConnectDisconnect;
    private boolean mMtuRequested;
    private byte []mUartData = new byte[6];
    private long mStartTimeImageTransfer;

    // File transfer variables
    private int mBytesTransfered = 0, mBytesTotal = 0;
    private byte []mDataBuffer;
    private boolean mStreamActive = false;
    private boolean mAccelActive = false;

    // Save all received data to a file with a timestamp
    File sdCard, dir;
    //Calendar calendar;
    //Date currentLocalTime;
    //DateFormat date, date_ms;
    String timestamp_ms, start_time_sys;
    String activity_log;
    long start_time;
    long fileCount;
    boolean frame;

    // Resolution variables
    int imgWidth = 100;
    int imgHeight = 100;

    public enum AppRunMode {Disconnected, Connected, ConnectedDuringSingleTransfer, ConnectedDuringStream};
    public enum BleCommand {NoCommand, StartSingleCapture, StartStreaming, StopStreaming, ChangeResolution, ChangePhy, GetBleParams, ChangeAngle, JPEG, StreamAccel, WriteReg};

    Handler guiUpdateHandler = new Handler();
    Runnable guiUpdateRunnable = new Runnable(){
        @Override
        public void run(){
            if(mTextViewFileLabel != null) {
                mTextViewFileLabel.setText("Incoming: " + String.valueOf(mBytesTransfered) + "/" + String.valueOf(mBytesTotal));
                if(mBytesTotal > 0) {
                    mProgressBarFileStatus.setProgress(mBytesTransfered * 100 / mBytesTotal);
                }
            }
            guiUpdateHandler.postDelayed(this, 50);
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBtAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        btnConnectDisconnect    = (Button) findViewById(R.id.btn_select);
        mTextViewLog = (TextView)findViewById(R.id.textViewLog);
        mTextViewFileLabel = (TextView)findViewById(R.id.textViewFileLabel);
        mTextViewPictureStatus = (TextView)findViewById(R.id.textViewImageStatus);
        mTextViewConInt = (TextView)findViewById(R.id.textViewCI);
        //mTextViewMtu = (TextView)findViewById(R.id.textViewMTU);
        mProgressBarFileStatus = (ProgressBar)findViewById(R.id.progressBarFile);
        mBtnTakePicture = (Button)findViewById(R.id.buttonTakePicture);
        mBtnStartStream = (Button)findViewById(R.id.buttonStartStream);
        mBtnStartAccel = (Button)findViewById(R.id.buttonStartAccel);
        mMainImage = (ImageView)findViewById(R.id.imageTransfered);
        mSpinnerResolution = (Spinner)findViewById(R.id.spinnerResolution);
        mSpinnerResolution.setSelection(0);
        mSpinnerPhy = (Spinner)findViewById(R.id.spinnerPhy);
        mSpinnerAngle = (Spinner)findViewById(R.id.spinnerAngle);
        mSpinnerAngle.setSelection(3);
        mFilename = (EditText)findViewById(R.id.filename);
        mJPEG = (CheckBox) findViewById(R.id.jpeg);
        mDebug = (CheckBox) findViewById(R.id.debug);
        mFlip = (CheckBox) findViewById(R.id.flip);
        fileCount=0;
        mSpinnerDevice = (Spinner)findViewById(R.id.spinnerDevice);
        mRegAddr = (EditText)findViewById(R.id.regAddr);
        mRegVal = (EditText)findViewById(R.id.regVal);
        mBtnWriteReg = (Button)findViewById(R.id.buttonSend);


        mMainImage.setScaleType(ImageView.ScaleType.FIT_XY);
        mMainImage.setAdjustViewBounds(true);

        // Save all received data to a file with a timestamp
        //calendar = Calendar.getInstance( TimeZone.getDefault(),
        //        Locale.getDefault());
        //currentLocalTime = calendar.getTime();
        //date = new SimpleDateFormat("hh_mm_ss",Locale.getDefault());
        //date_ms = new SimpleDateFormat("hh_mm_ss_SS",Locale.getDefault());

        try{
            sdCard = Environment.getExternalStorageDirectory();
        } catch(Exception e){
            Log.w(TAG, "Error accessing files");
            e.printStackTrace();
        }

        service_init();
        for(int i = 0; i < 6; i++) mUartData[i] = 0;


        // Handler Disconnect & Connect button
        btnConnectDisconnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mBtAdapter.isEnabled()) {
                    Log.i(TAG, "onClick - BT not enabled yet");
                    Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
                } else {
                    if (btnConnectDisconnect.getText().equals("Connect")) {

                        //Connect button pressed, open DeviceListActivity class, with popup windows that scan for devices

                        Intent newIntent = new Intent(MainActivity.this, DeviceListActivity.class);
                        startActivityForResult(newIntent, REQUEST_SELECT_DEVICE);
                    } else {
                        //Disconnect button pressed
                        if (mDevice != null) {
                            mService.disconnect();
                        }
                    }
                }
            }
        });

        mBtnTakePicture.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mMainImage.setImageResource(0);
                if(mService != null){
                    mService.sendCommand(BleCommand.StartSingleCapture.ordinal(), null);
                    setGuiByAppMode(AppRunMode.ConnectedDuringSingleTransfer);
                }
            }
        });

        mBtnStartStream.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mService != null){
                    if(!mStreamActive) {
                        mStreamActive = true;
                        mAccelActive = false;

                        mService.sendCommand(BleCommand.StartStreaming.ordinal(), null);
                        setGuiByAppMode(AppRunMode.ConnectedDuringStream);
                    }
                    else {
                        mStreamActive = false;
                        mAccelActive = false;

                        mService.sendCommand(BleCommand.StopStreaming.ordinal(), null);
                        setGuiByAppMode(AppRunMode.Connected);
                    }
                }
            }
        });

        mBtnStartAccel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mService != null){
                    if(!mStreamActive) {
                        mStreamActive = true;
                        mAccelActive = true;

                        mService.sendCommand(BleCommand.StreamAccel.ordinal(), null);
                        activity_log = mFilename.getText().toString();
                        String timestamp = DateFormat.getTimeInstance().format(new Date());
                        if(activity_log == null || activity_log.isEmpty()) {
                            activity_log = "activity_" + timestamp.replace(":", "_");
                        }

                        setGuiByAppMode(AppRunMode.ConnectedDuringStream);
                    }
                    else {
                        mStreamActive = false;
                        mAccelActive = false;

                        mService.sendCommand(BleCommand.StopStreaming.ordinal(), null);
                        setGuiByAppMode(AppRunMode.Connected);
                    }
                }
            }
        });

        mBtnWriteReg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mService != null && mService.isConnected()){
                    long device = mSpinnerDevice.getSelectedItemId();
                    byte[] regAddr = hexStringToByteArray(mRegAddr.getText().toString());
                    byte[] regVal =  hexStringToByteArray(mRegVal.getText().toString());
                    byte[] cmdData = new byte[4];
                    cmdData[0] = (byte) device;
                    cmdData[1] = regAddr[0];
                    cmdData[2] = regAddr[1];
                    cmdData[3] = regVal[0];
                    Log.w(TAG, "Sending: 0x" + Integer.toHexString(cmdData[0])+" 0x"+Integer.toHexString(cmdData[1])+" 0x"+Integer.toHexString(cmdData[2])+" 0x"+Integer.toHexString(cmdData[3]));
                    mService.sendCommand(BleCommand.WriteReg.ordinal(), cmdData);
                }
            }
        });

        mSpinnerResolution.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                Log.w(TAG, "Resolution selected: " + position);
                switch(position) {
                    case 0:
                        imgWidth = 100;
                        imgHeight = 100;
                        break;
                    case 1:
                        imgWidth = 160;
                        imgHeight = 120;
                        break;
                    case 2:
                        imgWidth = 160;
                        imgHeight = 160;
                        break;
                    case 3:
                        imgWidth = 200;
                        imgHeight = 160;
                        break;
                    default:
                        imgWidth = 100;
                        imgHeight = 100;
                }
                Log.w(TAG, "Resolution set to: "+imgWidth+"x"+imgHeight);

                if(mService != null && mService.isConnected()){
                    byte []cmdData = new byte[1];
                    cmdData[0] = (byte)position;
                    mService.sendCommand(BleCommand.ChangeResolution.ordinal(), cmdData);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {
            }
        });

        mSpinnerPhy.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                Log.w(TAG, "PHY selected: " + position);
                if(mService != null && mService.isConnected()){
                    byte []cmdData = new byte[1];
                    cmdData[0] = (byte)position;
                    mService.sendCommand(BleCommand.ChangePhy.ordinal(), cmdData);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {
            }
        });

        mSpinnerAngle.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                Log.w(TAG, "Angle selected: " + position);
                if(mService != null && mService.isConnected()){
                    byte []cmdData = new byte[1];
                    cmdData[0] = (byte)position;
                    mService.sendCommand(BleCommand.ChangeAngle.ordinal(), cmdData);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parentView) {
            }
        });

        /* Disable JPEG command
        mJPEG.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean checked = ((CheckBox) v).isChecked();
                Log.w(TAG, "Angle selected: " + checked);
                if(mService != null && mService.isConnected()){
                    byte []cmdData = new byte[1];
                    cmdData[0] = (byte) (checked ? 1 : 0);
                    mService.sendCommand(BleCommand.JPEG.ordinal(), cmdData);
                }

            }
        });
        */

        // Set initial UI state
        guiUpdateHandler.postDelayed(guiUpdateRunnable, 0);

        setGuiByAppMode(AppRunMode.Disconnected);
    }


    //UART service connected/disconnected
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder rawBinder) {
            mService = ((ImageTransferService.LocalBinder) rawBinder).getService();
            Log.d(TAG, "onServiceConnected mService= " + mService);
            if (!mService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
        }

        public void onServiceDisconnected(ComponentName classname) {
       ////     mService.disconnect(mDevice);
        		mService = null;
        }
    };

    private void setGuiByAppMode(AppRunMode appMode)
    {
        switch(appMode)
        {
            case Connected:
                mBtnTakePicture.setEnabled(true);
                mBtnStartStream.setEnabled(true);
                mBtnStartAccel.setEnabled(true);
                btnConnectDisconnect.setText("Disconnect");
                mBtnStartStream.setText("Start Stream");
                mBtnStartAccel.setText("Start Accel");
                mSpinnerResolution.setEnabled(true);
                mSpinnerPhy.setEnabled(true);
                mSpinnerAngle.setEnabled(true);
                mJPEG.setEnabled(true);
                mFlip.setEnabled(true);
                mDebug.setEnabled(true);
                mBtnWriteReg.setEnabled(true);
                mSpinnerDevice.setEnabled(true);
                mRegVal.setEnabled(true);
                mRegAddr.setEnabled(true);
                break;

            case Disconnected:
                mBtnTakePicture.setEnabled(false);
                mBtnStartStream.setEnabled(false);
                mBtnStartAccel.setEnabled(false);
                btnConnectDisconnect.setText("Connect");
                mBtnStartStream.setText("Start Stream");
                mBtnStartAccel.setText("Start Accel");
                mTextViewPictureStatus.setVisibility(View.INVISIBLE);
                mSpinnerResolution.setEnabled(false);
                mSpinnerResolution.setSelection(0);
                imgWidth = 100;
                imgHeight = 100;
                mSpinnerPhy.setEnabled(false);
                mSpinnerPhy.setSelection(0);
                mSpinnerAngle.setEnabled(false);
                mSpinnerAngle.setSelection(3);
                mJPEG.setEnabled(false);
                mFlip.setEnabled(false);
                mDebug.setEnabled(false);
                mBtnWriteReg.setEnabled(false);
                mSpinnerDevice.setEnabled(false);
                mRegVal.setEnabled(false);
                mRegAddr.setEnabled(false);
                break;

            case ConnectedDuringSingleTransfer:
                mBtnTakePicture.setEnabled(false);
                mBtnStartStream.setEnabled(false);
                mBtnStartAccel.setEnabled(false);
                mBtnWriteReg.setEnabled(false);
                mSpinnerDevice.setEnabled(false);
                mRegVal.setEnabled(false);
                mRegAddr.setEnabled(false);
                break;

            case ConnectedDuringStream:
                mBtnTakePicture.setEnabled(false);
                if(mAccelActive){
                    mBtnStartStream.setEnabled(false);
                    mBtnStartAccel.setEnabled(true);
                    mBtnStartAccel.setText("Stop Accel");
                } else {
                    mBtnStartAccel.setEnabled(false);
                    mBtnStartStream.setEnabled(true);
                    mBtnStartStream.setText("Stop Stream");
                }
                mSpinnerResolution.setEnabled(false);
                mSpinnerPhy.setEnabled(false);
                //mSpinnerAngle.setEnabled(false);
                mBtnWriteReg.setEnabled(false);
                mSpinnerDevice.setEnabled(false);
                mRegVal.setEnabled(false);
                mRegAddr.setEnabled(false);
                break;
        }
    }

    private void writeToLog(String message, AppLogFontType msgType){
        String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
        String newMessage = currentDateTimeString + " - " + message;
        String fontHtmlTag;
        switch(msgType){
            case APP_NORMAL:
                fontHtmlTag = "<font color='#000000'>";
                break;
            case APP_ERROR:
                fontHtmlTag = "<font color='#AA0000'>";
                break;
            case PEER_NORMAL:
                fontHtmlTag = "<font color='#0000AA'>";
                break;
            case PEER_ERROR:
                fontHtmlTag = "<font color='#FF00AA'>";
                break;
            default:
                fontHtmlTag = "<font>";
                break;
        }
        mLogMessage = fontHtmlTag + newMessage + "</font>" + "<br>" + mLogMessage;
        mTextViewLog.setText(Html.fromHtml(mLogMessage));
    }

    private final BroadcastReceiver UARTStatusChangeReceiver = new BroadcastReceiver() {
        public void onReceive(final Context context, Intent intent) {
        String action = intent.getAction();

        final Intent mIntent = intent;
        //*********************//
        if (action.equals(ImageTransferService.ACTION_GATT_CONNECTED)) {
            runOnUiThread(new Runnable() {
                public void run() {
                    mMtuRequested = false;
                    String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                    Log.d(TAG, "UART_CONNECT_MSG");
                    writeToLog("Connected", AppLogFontType.APP_NORMAL);
                }
            });
        }

          //*********************//
        if (action.equals(ImageTransferService.ACTION_GATT_DISCONNECTED)) {
            runOnUiThread(new Runnable() {
                public void run() {
                    String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                    Log.d(TAG, "UART_DISCONNECT_MSG");
                    setGuiByAppMode(AppRunMode.Disconnected);
                    writeToLog("Disconnected", AppLogFontType.APP_NORMAL);
                    mState = UART_PROFILE_DISCONNECTED;
                    mUartData[0] = mUartData[1] = mUartData[2] = mUartData[3] = mUartData[4] = mUartData[5] = 0;
                    mService.close();
                }
            });
        }

        //*********************//
        if (action.equals(ImageTransferService.ACTION_GATT_SERVICES_DISCOVERED)) {
            // Record time for logging
            start_time_sys = DateFormat.getTimeInstance().format(new Date());
            start_time = SystemClock.elapsedRealtime();

            mService.enableTXNotification();
            mService.sendCommand(BleCommand.GetBleParams.ordinal(), null);
            btnConnectDisconnect.setText("Disconnect");
            mTextViewPictureStatus.setText("Connected, updating params...");
            mTextViewPictureStatus.setVisibility(View.VISIBLE);
            //setGuiByAppMode(AppRunMode.Connected);

        }

        //*********************//
        if (action.equals(ImageTransferService.ACTION_DATA_AVAILABLE)) {

            final byte[] txValue = intent.getByteArrayExtra(ImageTransferService.EXTRA_DATA);
            runOnUiThread(new Runnable() {
            public void run() {
                try {
                    System.arraycopy(txValue, 0, mDataBuffer, mBytesTransfered, txValue.length);
                    if(mBytesTransfered == 0){
                        Log.w(TAG, "First packet received: " + String.valueOf(txValue.length) + " bytes");
                    }
                    mBytesTransfered += txValue.length;
                    if(mBytesTransfered >= mBytesTotal) {
                        long elapsedTime = System.currentTimeMillis() - mStartTimeImageTransfer;
                        float elapsedSeconds = (float) elapsedTime / 1000.0f;
                        DecimalFormat df = new DecimalFormat("0.0");
                        df.setMaximumFractionDigits(1);
                        String elapsedSecondsString = df.format(elapsedSeconds);
                        String kbpsString = df.format((float) mDataBuffer.length / elapsedSeconds * 8.0f / 1000.0f);
                        //writeToLog("Completed in " + elapsedSecondsString + " seconds. " + kbpsString + " kbps", AppLogFontType.APP_NORMAL);
                        if(mDebug.isChecked()){
                            try {
                                // MODE_APPEND, MODE_WORLD_READABLE, MODE_WORLD_WRITEABLE
                                // create new file or rewrite existing
                                // append to file
                                String rate = (elapsedTime+","+mDataBuffer.length+","+(float) mDataBuffer.length / elapsedSeconds * 8.0f / 1000.0f+"\r\n");
                                String filename = mFilename.getText().toString();
                                if(filename == null || filename.isEmpty()) {
                                    String timestamp = DateFormat.getTimeInstance().format(new Date());
                                    filename = "rate_" +start_time_sys.replace(":","_")+".txt";
                                }

                                File dir = new File(sdCard.getAbsolutePath() + "/Download/bug_cam/rate/");
                                dir.mkdirs();
                                File file0 = new File(dir, filename+".txt");
                                FileOutputStream fos = new FileOutputStream(file0, true);
                                fos.write(rate.getBytes(StandardCharsets.UTF_8));
                                fos.close();
                            } catch (IOException e) {
                                e.toString();
                            }
                        }

                        /**************************** Check data **********************************/
                        //mTextViewPictureStatus.setText(String.valueOf(mDataBuffer.length / 1024) + "kB - " + elapsedSecondsString + " seconds - " + kbpsString + " kbps");
                        //mTextViewPictureStatus.setVisibility(View.VISIBLE);

                        // Print the first 10 bytes received
                        StringBuilder sb = new StringBuilder(60);
                        for (int i = 0; i < 10; i++) {
                            sb.append(String.format("0x%02X ", mDataBuffer[i]));
                        }

                        Log.w(TAG, "Check first 10 bytes: " + sb.toString());
                        mTextViewPictureStatus.setText(String.valueOf(mDataBuffer.length / 1024) + "kB - " + elapsedSecondsString + " seconds - " + kbpsString + " kbps" + "\nCheck first 10 bytes: " + sb.toString());

                        // Show first 10 bytes
                        if(mDebug.isChecked())
                            mTextViewPictureStatus.setVisibility(View.VISIBLE);
                        else
                            mTextViewPictureStatus.setVisibility(View.INVISIBLE);

                        // Save all received data to a file with a timestamp
                        //currentLocalTime = calendar.getTime();
                        //timestamp = date.format(currentLocalTime);
                        //timestamp_ms = date_ms.format(currentLocalTime);
                        timestamp_ms = DateFormat.getTimeInstance().format(new Date());

                        // Log timestamp to file
                        if(mDebug.isChecked()){
                            try{
                                long elapsedMilliSeconds = SystemClock.elapsedRealtime() - start_time;
                                File dir = new File(sdCard.getAbsolutePath() + "/Download/bug_cam/timestamps/");
                                dir.mkdirs();
                                File file_debug = new File(dir, "start_"+start_time_sys.replace(":","_")+".txt");
                                FileOutputStream f_debug = new FileOutputStream(file_debug);
                                f_debug.write(("Connected at: "+start_time_sys+"\r\n").getBytes(StandardCharsets.UTF_8));
                                f_debug.write(("Last frame sent: "+timestamp_ms+"\r\n").getBytes(StandardCharsets.UTF_8));
                                f_debug.write(("System time (ms): "+System.currentTimeMillis()+"\r\n").getBytes(StandardCharsets.UTF_8));
                                f_debug.write(("Elapsed time (ms): "+elapsedMilliSeconds+"\r\n").getBytes(StandardCharsets.UTF_8));
                                f_debug.close();
                            } catch(Exception e){
                                Log.w(TAG, "Error logging timestamp");
                                e.printStackTrace();
                            }
                        }

                        if (frame && (!mStreamActive || mJPEG.isChecked())) {
                            try {
                                String filename = mFilename.getText().toString();
                                String timestamp = DateFormat.getTimeInstance().format(new Date());
                                if(filename == null || filename.isEmpty()) {
                                    filename = "img_data_" + timestamp.replace(":", "_");
                                }

                                Log.w(TAG, "Saving image to " + sdCard.getAbsolutePath()
                                        + "/Download/bug_cam/"+filename);

                                // Append timestamp of accelerometer interrupt to log
                                if(mAccelActive) {
                                    String activity_log_path = sdCard.getAbsolutePath() + "/Download/bug_cam/timestamps/";
                                    File dir = new File(activity_log_path);
                                    dir.mkdirs();

                                    File file_accel = new File(activity_log_path+activity_log+".txt");
                                    FileOutputStream fos = new FileOutputStream(file_accel, true);
                                    fos.write((System.currentTimeMillis()+", "+timestamp+"\r\n").getBytes(StandardCharsets.UTF_8));
                                    fos.close();
                                }


                                // Save the image
                                File dir;
                                if(mStreamActive){
                                    dir = new File(sdCard.getAbsolutePath() + "/Download/bug_cam/"+ filename);
                                } else {
                                    dir = new File(sdCard.getAbsolutePath() + "/Download/bug_cam/");
                                }
                                dir.mkdirs();
                                filename = filename + fileCount;
                                fileCount++;
                                File file1 = new File(dir, filename);
                                FileOutputStream f1 = new FileOutputStream(file1);
                                f1.write(mDataBuffer);
                                f1.close();
                                frame = false;

                            } catch(Exception e){
                                Log.w(TAG, "Error writing image data to file");
                                e.printStackTrace();
                            }
                        }

                        // Set to false to disable compression
                        float[] mx = new float[]{
                                1.0f, 0, 0, 0, 0, //red
                                0, 1.0f, 0, 0, 0, //green
                                0, 0, 1.0f, 0, 0, //blue
                                0, 0, 0, -1.0f, 255 //alpha
                        };
                        ColorMatrixColorFilter cf = new ColorMatrixColorFilter(mx);

                        Log.w(TAG, "Length of data: "+mDataBuffer.length);
                        Log.w(TAG, "attempting image display");
                        Log.w(TAG, "Image size: "+imgWidth+"x"+imgHeight);
                        try {
                            Bitmap bitmap;
                            /* Remove JPEG
                            if(mJPEG.isChecked()){
                                // Original code with JPEG compression
                                bitmap = BitmapFactory.decodeByteArray(mDataBuffer, 0, mDataBuffer.length);
                            } else {
                            */
                                mMainImage.setColorFilter(cf);

                                //bitmap = Bitmap.createBitmap(326 , 160, Bitmap.Config.ALPHA_8);
                                //bitmap = Bitmap.createBitmap(220, 120, Bitmap.Config.ALPHA_8);
                                //bitmap = Bitmap.createBitmap(100, 60, Bitmap.Config.ALPHA_8);
                                //if(mDataBuffer.length > imgHeight*imgWidth){
                                //    bitmap = Bitmap.createBitmap(imgWidth, imgHeight+1, Bitmap.Config.ALPHA_8);
                                //} else {
                                    bitmap = Bitmap.createBitmap(imgWidth, imgHeight, Bitmap.Config.ALPHA_8);
                                //}
                                bitmap.copyPixelsFromBuffer(ByteBuffer.wrap(mDataBuffer));

                            //}
                            if(mFlip.isChecked()){
                                float degrees = 180;//rotation degree
                                Matrix matrix = new Matrix();
                                matrix.setRotate(degrees);
                                bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
                            }
                            mMainImage.setImageBitmap(bitmap);
                        } catch (Exception e) {
                            Log.w(TAG, "Bitmapfactory fail :(");
                        }
                        /**************************************************************************/

                        if(!mStreamActive) {
                            setGuiByAppMode(AppRunMode.Connected);
                        }
                    }
                } catch (Exception e) {
                    Log.e(TAG, e.toString());
                }
            }
            });
        }
        //*********************//
        if (action.equals(ImageTransferService.ACTION_IMG_INFO_AVAILABLE)) {
            final byte[] txValue = intent.getByteArrayExtra(ImageTransferService.EXTRA_DATA);
            runOnUiThread(new Runnable() {
                public void run() {
                    try {
                        switch(txValue[0]) {
                            case 1:
                                // Start a new file transfer
                                ByteBuffer byteBuffer = ByteBuffer.wrap(Arrays.copyOfRange(txValue, 1, 5));
                                byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
                                int fileSize = byteBuffer.getInt();
                                mBytesTotal = fileSize;
                                mDataBuffer = new byte[fileSize];
                                mTextViewFileLabel.setText("Incoming file: " + String.valueOf(fileSize) + " bytes.");
                                mBytesTransfered = 0;
                                mStartTimeImageTransfer = System.currentTimeMillis();
                                frame = true;
                                break;

                            case 2:
                                ByteBuffer mtuBB = ByteBuffer.wrap(Arrays.copyOfRange(txValue, 1, 3));
                                mtuBB.order(ByteOrder.LITTLE_ENDIAN);
                                short mtu = mtuBB.getShort();
                                //mTextViewMtu.setText(String.valueOf(mtu) + " bytes");
                                if(!mMtuRequested && mtu < 64){
                                    mService.requestMtu(247);
                                    writeToLog("Requesting 247 byte MTU from app", AppLogFontType.APP_NORMAL);
                                    mMtuRequested = true;
                                }
                                ByteBuffer ciBB = ByteBuffer.wrap(Arrays.copyOfRange(txValue, 3, 5));
                                ciBB.order(ByteOrder.LITTLE_ENDIAN);
                                short conInterval = ciBB.getShort();
                                mTextViewConInt.setText(String.valueOf((float)conInterval * 1.25f) + "ms");
                                short txPhy = txValue[5];
                                short rxPhy = txValue[6];
                                if(txPhy == 0x0001 && mSpinnerPhy.getSelectedItemPosition() == 1) {
                                    mSpinnerPhy.setSelection(0);
                                    writeToLog("2Mbps not supported!", AppLogFontType.APP_ERROR);
                                }
                                else {
                                    mTextViewPictureStatus.setVisibility(View.INVISIBLE);
                                    setGuiByAppMode(AppRunMode.Connected);
                                    writeToLog("Parameters updated.", AppLogFontType.APP_NORMAL);
                                }
                                break;
                        }
                    } catch (Exception e) {
                        Log.e(TAG, e.toString());
                    }
                }
            });
        }
        //*********************//
        if (action.equals(ImageTransferService.DEVICE_DOES_NOT_SUPPORT_IMAGE_TRANSFER)){
            //showMessage("Device doesn't support UART. Disconnecting");
            writeToLog("APP: Invalid BLE service, disconnecting!",  AppLogFontType.APP_ERROR);
            mService.disconnect();
        }
        }
    };

    private void service_init() {
        Intent bindIntent = new Intent(this, ImageTransferService.class);
        bindService(bindIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
  
        LocalBroadcastManager.getInstance(this).registerReceiver(UARTStatusChangeReceiver, makeGattUpdateIntentFilter());
    }
    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ImageTransferService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(ImageTransferService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(ImageTransferService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(ImageTransferService.ACTION_DATA_AVAILABLE);
        intentFilter.addAction(ImageTransferService.ACTION_IMG_INFO_AVAILABLE);
        intentFilter.addAction(ImageTransferService.DEVICE_DOES_NOT_SUPPORT_IMAGE_TRANSFER);
        return intentFilter;
    }
    @Override
    public void onStart() {
        super.onStart();
    }

    @Override
    public void onDestroy() {
    	 super.onDestroy();
        Log.d(TAG, "onDestroy()");
        
        try {
        	LocalBroadcastManager.getInstance(this).unregisterReceiver(UARTStatusChangeReceiver);
        } catch (Exception ignore) {
            Log.e(TAG, ignore.toString());
        } 
        unbindService(mServiceConnection);
        mService.stopSelf();
        mService= null;
       
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "onStop");
        super.onStop();
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause");
        super.onPause();
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart");
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        if (!mBtAdapter.isEnabled()) {
            Log.i(TAG, "onResume - BT not enabled yet");
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
 
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_SELECT_DEVICE:
                //When the DeviceListActivity return, with the selected device address
                if (resultCode == Activity.RESULT_OK && data != null) {
                    String deviceAddress = data.getStringExtra(BluetoothDevice.EXTRA_DEVICE);
                    mDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(deviceAddress);

                    Log.d(TAG, "... onActivityResultdevice.address==" + mDevice + "mserviceValue" + mService);
                    //((TextView) findViewById(R.id.deviceName)).setText(mDevice.getName()+ " - connecting");
                    mService.connect(deviceAddress);
                }
                break;

            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    Toast.makeText(this, "Bluetooth has turned on ", Toast.LENGTH_SHORT).show();

                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "BT not enabled");
                    Toast.makeText(this, "Problem in BT Turning ON ", Toast.LENGTH_SHORT).show();
                    finish();
                }
                break;

            default:
                Log.e(TAG, "wrong request code");
                break;
        }
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
       
    }

    
    private void showMessage(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
  
    }

    @Override
    public void onBackPressed() {
        if (mState == UART_PROFILE_CONNECTED) {
            Intent startMain = new Intent(Intent.ACTION_MAIN);
            startMain.addCategory(Intent.CATEGORY_HOME);
            startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(startMain);
            showMessage("nRFUART's running in background.\n             Disconnect to exit");
        }
        else {
            finish();
        }
    }

    /* Checks if external storage is available for read and write */
    public boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

}
