#include <mbed.h>
#include "ble/BLE.h"
#include "gap/Gap.h"
#include "gap/AdvertisingDataParser.h"
#include "pretty_printer.h"

EventQueue event_queue;

typedef struct {
    ble::scan_interval_t interval;
    ble::scan_window_t   window;
    ble::scan_duration_t duration;
    bool active;
} GapScanParam_t;

/** the entries in this array are used to configure our scanning
 *  parameters for each of the modes we use in our demo */
static const GapScanParam_t scanning_params[] = {
/*                      interval                  window                   duration  active */
/*                      0.625ms                  0.625ms                       10ms         */
    {   ble::scan_interval_t(160),   ble::scan_window_t(100),   ble::scan_duration_t(300), false },
    { ble::scan_interval_t(160), ble::scan_window_t(100), ble::scan_duration_t(300), false },
    { ble::scan_interval_t(160),  ble::scan_window_t(40),   ble::scan_duration_t(0), true  },
    { ble::scan_interval_t(500),  ble::scan_window_t(10),   ble::scan_duration_t(0), false }
};

/** Demonstrate scanning
 */
class GapScanner : private mbed::NonCopyable<GapScanner>, public ble::Gap::EventHandler {
public:
    GapScanner(BLE& ble, events::EventQueue& event_queue) :
        _ble(ble),
        _gap(ble.gap()),
        _event_queue(event_queue),
        _led1(LED1, 0),
        _set_index(0),        
        _scan_count(0),
        _blink_event(0) {
    }

    ~GapScanner() {
        if (_ble.hasInitialized()) {
            _ble.shutdown();
        }
    }

/** Start BLE interface initialisation */
    void run() {
        if (_ble.hasInitialized()) {
            printf("Ble instance already initialised.\r\n");
            return;
        }

        /* handle gap events */
        _gap.setEventHandler(this);

        ble_error_t error = _ble.init(this, &GapScanner::on_init_complete);
        if (error) {
            print_error(error, "Error returned by BLE::init");
            return;
        }

        /* to show we're running we'll blink every 500ms */
        _blink_event = _event_queue.call_every(500, this, &GapScanner::blink);

        /* this will not return until shutdown */
        _event_queue.dispatch_forever();
    }

private:
    /** This is called when BLE interface is initialised and starts the first mode */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *event) {
        if (event->error) {
            print_error(event->error, "Error during the initialisation");
            return;
        }

        print_mac_address();

        /* setup the default phy used in connection to 2M to reduce power consumption */
        if (_gap.isFeatureSupported(ble::controller_supported_features_t::LE_2M_PHY)) {
            ble::phy_set_t phys(/* 1M */ false, /* 2M */ true, /* coded */ false);

            ble_error_t error = _gap.setPreferredPhys(/* tx */&phys, /* rx */&phys);
            if (error) {
                print_error(error, "GAP::setPreferedPhys failed");
            }
        }

        /* all calls are serialised on the user thread through the event queue */
        _event_queue.call(this, &GapScanner::scan);
    }

 /** Set up and start scanning */
    void scan() {
        const GapScanParam_t &scan_params = scanning_params[_set_index];

        /*
         * Scanning happens repeatedly and is defined by:
         *  - The scan interval which is the time (in 0.625us) between each scan cycle.
         *  - The scan window which is the scanning time (in 0.625us) during a cycle.
         * If the scanning process is active, the local device sends scan requests
         * to discovered peer to get additional data.
         */
        ble_error_t error = _gap.setScanParameters(
            ble::ScanParameters(
                ble::phy_t::LE_1M,   // scan on the 1M PHY
                scan_params.interval,
                scan_params.window,
                scan_params.active
            )
        );
        if (error) {
            print_error(error, "Error caused by Gap::setScanParameters");
            return;
        }

        /* start scanning and attach a callback that will handle advertisements
         * and scan requests responses */
        error = _gap.startScan(scan_params.duration);
        if (error) {
            print_error(error, "Error caused by Gap::startScan");
            return;
        }

        printf("Scanning started (interval: %dms, window: %dms, timeout: %dms).\r\n",
               scan_params.interval.valueInMs(), scan_params.window.valueInMs(), scan_params.duration.valueInMs());        
    }

    /** Blink LED to show we're running */
    void blink(void) {
        _led1 = !_led1;
    }

private:
    /* Gap::EventHandler */

    /** Look at scan payload to find a peer device and connect to it */
    virtual void onAdvertisingReport(const ble::AdvertisingReportEvent &event) {
        /* keep track of scan events for performance reporting */
        _scan_count++;

        printf("Found a device at %d\n", event.getRssi());

        const uint8_t* addr = event.getPeerAddress().data();

        printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    
    }

    virtual void onScanTimeout(const ble::ScanTimeoutEvent&) {
        printf("Stopped scanning early due to timeout parameter\r\n");
        _event_queue.call_in(10000, this, &GapScanner::scan);
    }    

private:
    BLE                &_ble;
    ble::Gap           &_gap;
    EventQueue         &_event_queue;
    DigitalOut          _led1;

    /* Keep track of our progress through demo modes */
    size_t              _set_index;

    /* Measure performance of our advertising/scanning */
    size_t              _scan_count;

    int                 _blink_event;
};

/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

int main()
{
    BLE &ble = BLE::Instance();

    /* this will inform us off all events so we can schedule their handling
     * using our event queue */
    printf("Starting\n");
    ble.onEventsToProcess(schedule_ble_events);
    GapScanner scanner(ble, event_queue);

    scanner.run();

    return 0;
}