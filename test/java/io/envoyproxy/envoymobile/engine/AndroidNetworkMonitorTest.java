package io.envoyproxy.envoymobile.engine;

class AndroidNetworkMonitorTest {

  private AndroidNetworkMonitor;
  private ShadowConnectivityManager;

  @Before
  public setUp() {
    androidNetworkMonitor = AndroidNetworkMonitor.load(); //mReceiver.register();
    mConnectivityManger = new ShadowConnectivityManager();
    androidNetworkMonitor.setConnectivityManagerForTesting(mConnectivityManager);
  }
  /**
   * Tests that isOnline() returns the correct result.
   */
  @Test
  @UiThreadTest
  @MediumTest
  public void testNetworkChangeNotifierIsOnline() {
    Intent intent = new Intent(ConnectivityManager.CONNECTIVITY_ACTION);
    // For any connection type it should return true.
    for (int i = ConnectivityManager.TYPE_MOBILE; i < ConnectivityManager.TYPE_VPN; i++) {
      Assert.assertTrue(NetworkChangeNotifier.isOnline());
    }
    mConnectivityManager.setDefaultNetworkActive(false);
    mReceiver.onReceive(InstrumentationRegistry.getTargetContext(), intent);
    Assert.assertFalse(NetworkChangeNotifier.isOnline());
  }
}