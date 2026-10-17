/*
 * SDL Android Java support classes from SDL 2.32.10.
 * Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>
 * Licensed under the zlib license; see android/licenses/SDL2-LICENSE.txt.
 */
package org.libsdl.app;

import android.hardware.usb.UsbDevice;

interface HIDDevice
{
    public int getId();
    public int getVendorId();
    public int getProductId();
    public String getSerialNumber();
    public int getVersion();
    public String getManufacturerName();
    public String getProductName();
    public UsbDevice getDevice();
    public boolean open();
    public int sendFeatureReport(byte[] report);
    public int sendOutputReport(byte[] report);
    public boolean getFeatureReport(byte[] report);
    public void setFrozen(boolean frozen);
    public void close();
    public void shutdown();
}
