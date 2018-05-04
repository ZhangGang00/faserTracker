#include "FaserTracker/common_includes.hh"
#include "FaserTracker/DigiCluster.hh"


namespace FaserTracker {

    void DigiCluster::addDigit(const Digit & digit, bool isTruth) {

        if (eventNumber > -1 && digit.eventNumber != eventNumber) {
            cout << "WARNING  DigiCluster::addDigit\n"
                 << "         Adding `Digit` to `DigiCluster` with different event number\n"
                 << "         Updating `DigiCluster` event number\n";
        }

        if (digit.plane != plane) {
            cout << "WARNING  DigiCluster::addDigit\n"
                 << "         `Digit` and `DigiCluster` have different plane numbers.\n";
        }

        if (isTruth) {
            if (truthTrackId > -1 && digit.truthTrackId != truthTrackId) {
                cout << "ERROR  DigiCluster::addDigit\n"
                     << "       Attempting to add `Digit` to `DigiCluster` with different `truthTrackId`\n";

                throw runtime_error {"invalid digit"};
            }

            truthTrackId = digit.truthTrackId;
        }

        eventNumber = digit.eventNumber;
        charge += digit.charge;

        digits->push_back(digit);

    }

    ////////////////////////////////////////////////////////////////////////////////

    shared_ptr<TVector3> DigiCluster::globalPosition() const {

        if (_cachedGlobalPosition != nullptr) {
            return _cachedGlobalPosition;
        }

        double totalCharge = 0;
        auto centerOfCharge = make_shared<TVector3>();
        for (const Digit & d : *digits) {
            totalCharge += d.charge;
            *centerOfCharge += d.charge * *d.globalPosition();
        }
        *centerOfCharge *= 1.0/totalCharge;

        _cachedGlobalPosition = centerOfCharge;
        return centerOfCharge;

    }

    ////////////////////////////////////////////////////////////////////////////////

    void DigiCluster::print() const {

        shared_ptr<TVector3> pos = globalPosition();

        cout << "        DigiCluster  plane=" << plane
             <<                    "  nDigits=" << digits->size()
             <<                    "  globalPos=(" << pos->X()
             <<                               ", " << pos->Y()
             <<                               ", " << pos->Z() << ")\n";

    }

    ////////////////////////////////////////////////////////////////////////////////

    void DigiCluster::printTruthTrackIds() const {

        cout << "    Truth track IDs in `DigiCluster::digits` in plane " << plane << "\n"
             << "    ";

        for (const Digit & digit : *digits) {
            cout << "  " << digit.truthTrackId;
        }

        cout << "\n";

    }

}

