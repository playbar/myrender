/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: G:\\projects\\MJ\\MojingSDK\\Core\\VrService\\src\\com\\baofeng\\mojing\\service\\MojingSDKAIDLService.aidl
 */
package com.baofeng.mojing.service;
public interface MojingSDKAIDLService extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.baofeng.mojing.service.MojingSDKAIDLService
{
private static final java.lang.String DESCRIPTOR = "com.baofeng.mojing.service.MojingSDKAIDLService";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.baofeng.mojing.service.MojingSDKAIDLService interface,
 * generating a proxy if needed.
 */
public static com.baofeng.mojing.service.MojingSDKAIDLService asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.baofeng.mojing.service.MojingSDKAIDLService))) {
return ((com.baofeng.mojing.service.MojingSDKAIDLService)iin);
}
return new com.baofeng.mojing.service.MojingSDKAIDLService.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_clientResume:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
com.baofeng.mojing.service.IServiceResponse _arg1;
_arg1 = com.baofeng.mojing.service.IServiceResponse.Stub.asInterface(data.readStrongBinder());
this.clientResume(_arg0, _arg1);
reply.writeNoException();
return true;
}
case TRANSACTION_clientPause:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
this.clientPause(_arg0);
reply.writeNoException();
return true;
}
case TRANSACTION_getVID:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getVID();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getPID:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getPID();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getMCUVersion:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getMCUVersion();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getBLEVersion:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getBLEVersion();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getSN:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _result = this.getSN();
reply.writeNoException();
reply.writeString(_result);
return true;
}
case TRANSACTION_startUpgrade:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
boolean _result = this.startUpgrade(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_getUpgradeProgress:
{
data.enforceInterface(DESCRIPTOR);
float _result = this.getUpgradeProgress();
reply.writeNoException();
reply.writeFloat(_result);
return true;
}
case TRANSACTION_isUpgrading:
{
data.enforceInterface(DESCRIPTOR);
boolean _result = this.isUpgrading();
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_getHMDJoystickState:
{
data.enforceInterface(DESCRIPTOR);
boolean _result = this.getHMDJoystickState();
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_getHMDLowPowerState:
{
data.enforceInterface(DESCRIPTOR);
boolean _result = this.getHMDLowPowerState();
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_getSkinList:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _result = this.getSkinList();
reply.writeNoException();
reply.writeString(_result);
return true;
}
case TRANSACTION_loadOneSkin:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
int _result = this.loadOneSkin(_arg0);
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getControllerList:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _result = this.getControllerList();
reply.writeNoException();
reply.writeString(_result);
return true;
}
case TRANSACTION_connectController:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
boolean _result = this.connectController(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_disconnectController:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
boolean _result = this.disconnectController(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_isControllerConnected:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
boolean _result = this.isControllerConnected(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_addSocketTarget:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
this.addSocketTarget(_arg0);
reply.writeNoException();
return true;
}
case TRANSACTION_removeSocketTarget:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
this.removeSocketTarget(_arg0);
reply.writeNoException();
return true;
}
case TRANSACTION_socketHeartbeat:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
this.socketHeartbeat(_arg0);
reply.writeNoException();
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.baofeng.mojing.service.MojingSDKAIDLService
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public void clientResume(java.lang.String params, com.baofeng.mojing.service.IServiceResponse callback) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(params);
_data.writeStrongBinder((((callback!=null))?(callback.asBinder()):(null)));
mRemote.transact(Stub.TRANSACTION_clientResume, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void clientPause(java.lang.String params) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(params);
mRemote.transact(Stub.TRANSACTION_clientPause, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public int getVID() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getVID, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int getPID() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getPID, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int getMCUVersion() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getMCUVersion, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int getBLEVersion() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getBLEVersion, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public java.lang.String getSN() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
java.lang.String _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getSN, _data, _reply, 0);
_reply.readException();
_result = _reply.readString();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean startUpgrade(java.lang.String path) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(path);
mRemote.transact(Stub.TRANSACTION_startUpgrade, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public float getUpgradeProgress() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
float _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getUpgradeProgress, _data, _reply, 0);
_reply.readException();
_result = _reply.readFloat();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean isUpgrading() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_isUpgrading, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean getHMDJoystickState() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getHMDJoystickState, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean getHMDLowPowerState() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getHMDLowPowerState, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public java.lang.String getSkinList() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
java.lang.String _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getSkinList, _data, _reply, 0);
_reply.readException();
_result = _reply.readString();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int loadOneSkin(java.lang.String skinName) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(skinName);
mRemote.transact(Stub.TRANSACTION_loadOneSkin, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public java.lang.String getControllerList() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
java.lang.String _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getControllerList, _data, _reply, 0);
_reply.readException();
_result = _reply.readString();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean connectController(int id) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(id);
mRemote.transact(Stub.TRANSACTION_connectController, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean disconnectController(int id) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(id);
mRemote.transact(Stub.TRANSACTION_disconnectController, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public boolean isControllerConnected(int id) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(id);
mRemote.transact(Stub.TRANSACTION_isControllerConnected, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public void addSocketTarget(int port) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(port);
mRemote.transact(Stub.TRANSACTION_addSocketTarget, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void removeSocketTarget(int port) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(port);
mRemote.transact(Stub.TRANSACTION_removeSocketTarget, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void socketHeartbeat(int port) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(port);
mRemote.transact(Stub.TRANSACTION_socketHeartbeat, _data, _reply, 0);
_reply.readException();
}
finally {
_reply.recycle();
_data.recycle();
}
}
}
static final int TRANSACTION_clientResume = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_clientPause = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_getVID = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
static final int TRANSACTION_getPID = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
static final int TRANSACTION_getMCUVersion = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
static final int TRANSACTION_getBLEVersion = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
static final int TRANSACTION_getSN = (android.os.IBinder.FIRST_CALL_TRANSACTION + 6);
static final int TRANSACTION_startUpgrade = (android.os.IBinder.FIRST_CALL_TRANSACTION + 7);
static final int TRANSACTION_getUpgradeProgress = (android.os.IBinder.FIRST_CALL_TRANSACTION + 8);
static final int TRANSACTION_isUpgrading = (android.os.IBinder.FIRST_CALL_TRANSACTION + 9);
static final int TRANSACTION_getHMDJoystickState = (android.os.IBinder.FIRST_CALL_TRANSACTION + 10);
static final int TRANSACTION_getHMDLowPowerState = (android.os.IBinder.FIRST_CALL_TRANSACTION + 11);
static final int TRANSACTION_getSkinList = (android.os.IBinder.FIRST_CALL_TRANSACTION + 12);
static final int TRANSACTION_loadOneSkin = (android.os.IBinder.FIRST_CALL_TRANSACTION + 13);
static final int TRANSACTION_getControllerList = (android.os.IBinder.FIRST_CALL_TRANSACTION + 14);
static final int TRANSACTION_connectController = (android.os.IBinder.FIRST_CALL_TRANSACTION + 15);
static final int TRANSACTION_disconnectController = (android.os.IBinder.FIRST_CALL_TRANSACTION + 16);
static final int TRANSACTION_isControllerConnected = (android.os.IBinder.FIRST_CALL_TRANSACTION + 17);
static final int TRANSACTION_addSocketTarget = (android.os.IBinder.FIRST_CALL_TRANSACTION + 18);
static final int TRANSACTION_removeSocketTarget = (android.os.IBinder.FIRST_CALL_TRANSACTION + 19);
static final int TRANSACTION_socketHeartbeat = (android.os.IBinder.FIRST_CALL_TRANSACTION + 20);
}
public void clientResume(java.lang.String params, com.baofeng.mojing.service.IServiceResponse callback) throws android.os.RemoteException;
public void clientPause(java.lang.String params) throws android.os.RemoteException;
public int getVID() throws android.os.RemoteException;
public int getPID() throws android.os.RemoteException;
public int getMCUVersion() throws android.os.RemoteException;
public int getBLEVersion() throws android.os.RemoteException;
public java.lang.String getSN() throws android.os.RemoteException;
public boolean startUpgrade(java.lang.String path) throws android.os.RemoteException;
public float getUpgradeProgress() throws android.os.RemoteException;
public boolean isUpgrading() throws android.os.RemoteException;
public boolean getHMDJoystickState() throws android.os.RemoteException;
public boolean getHMDLowPowerState() throws android.os.RemoteException;
public java.lang.String getSkinList() throws android.os.RemoteException;
public int loadOneSkin(java.lang.String skinName) throws android.os.RemoteException;
public java.lang.String getControllerList() throws android.os.RemoteException;
public boolean connectController(int id) throws android.os.RemoteException;
public boolean disconnectController(int id) throws android.os.RemoteException;
public boolean isControllerConnected(int id) throws android.os.RemoteException;
public void addSocketTarget(int port) throws android.os.RemoteException;
public void removeSocketTarget(int port) throws android.os.RemoteException;
public void socketHeartbeat(int port) throws android.os.RemoteException;
}
