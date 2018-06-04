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

    ////////////////////////////////////////////////////////////////////////////////

    void DigiCluster::extractParameters() {
        // set `parameters` and `covarianceMatrix`
        // with Karimaki components rho, phi, d, lambda, z0
        //
        // NB: In this function we work with the *Karimaki* parameters:
        //     (x,y,z)_Karimaki = (z,x,y)_FASER

        bool searchInitialized = false;
        double zClosestApproach = 1e20;
        double yClosestApproach = 1e20;
        // We're dealing with clusters here, so this approach should avoid to outlier points.
        // TODO: Maybe use a distance tolerance to fitted circle.
        for (const DigiCluster & cluster : *digits) {
            shared_ptr<TVector3> pos = cluster.globalPosition();
            double y = pos->Z(); // y_Karimaki = z_FASER
            if (!searchInitialized ||
                isUpperSemicircle && y > yClosestApproach ||
                !isUpperSemicircle && y < yClosestApproach)
            {
                yClosestApproach = y;
                zClosestApproach = pos->Y(); // z_Karimaki = y_FASER
            }
        }

        // parameters
        translatedParameters[0] = rho;
        translatedParameters[1] = 0.;  // phi
        translatedParameters[2] = 0.;  // d
        // TODO: Lambda is not really zero --> determine from straight line fit?
        translatedParameters[3] = 0.;  // lambda
        translatedParameters[4] = zClosestApproach;  // "z0"


        // onto covariance matrix elements

        double sumOfWeights = 0.;

        // NB: The coordinate names here refer to the Karimaki coordinates, *not* the FASER coordinates.
        //     i.e. x_Karimaki = z_FASER, y_Karimaki = x_FASER
        // TODO: Figure out how to determine the curvative without (?) doing a global regression fit.
        double x_av     = 0.;
        double x2_av    = 0.;
        double y_av     = 0.;
        double y2_av    = 0.;
        double r_av     = 0.;
        double r2_av    = 0.;
        double r4_av    = 0.;
        double xr2_av   = 0.;
        double x2r4_av  = 0.;
        double z2_av    = 0.;
        double z4_av    = 0.;
        double zr_av    = 0.;
        double z2r2_av  = 0.;
        double sumOfInvDeltaZ2 = 0.;

        double sigma_x_x   = 0.;
        double sigma_r_r   = 0.;
        double sigma_x_r2  = 0.;
        double sigma_r2_r2 = 0.;

        bool zZeroPresent = false;
        for (const Digit & digit : *digits) {

            shared_ptr<TVector3> pos = digit.globalPosition();
            // NB: (x,y,z)_Karimaki = (z,x,y)_FASER
            double x = pos->Z();
            double y = pos->X();
            double z = pos->Y();
            double r = TMath::Sqrt(x*x + y*y);
            double w = digit.charge;

            sumOfWeights += w;
            x_av    += w*x;
            x2_av   += w*x*x;
            y_av    += w*y;
            y2_av   += w*y*y;
            r_av    += w*r;
            r2_av   += w*r*r;
            r4_av   += w*r*r*r*r;
            xr2_av  += w*x*r*r;
            z2_av   += w*z*z;
            z4_av   += w*z*z*z*z;
            zr_av   += w*z*r;
            z2r2_av += w*z*z*r*r;

            if (fabs(z) < 1e-20) {
                zZeroPresent = true;
            } else {
                sumOfInvDeltaZ2 += 1./(z*z);
            }

        }

        x_av    /= sumOfWeights;
        x2_av   /= sumOfWeights;
        y_av    /= sumOfWeights;
        y2_av   /= sumOfWeights;
        r_av    /= sumOfWeights;
        r2_av   /= sumOfWeights;
        r4_av   /= sumOfWeights;
        xr2_av  /= sumOfWeights;
        x2r4_av /= sumOfWeights;
        z2_av   /= sumOfWeights;
        z4_av   /= sumOfWeights;
        zr_av   /= sumOfWeights;
        z2r2_av /= sumOfWeights;

        double sigma_x_x   = x2_av - x_av*x_av;
        double sigma_r_r   = r2_av - r_av*r_av;
        double sigma_x_r2  = x2r4_av - xr2_av*xr2_av;
        double sigma_r2_r2 = r4_av - r2_av*r2_av;
        double sigma_z_z   = z4_av - z2_av*z2_av;
        double sigma_z_r   = z2r2_av - zr_av*zr_av;

        double C  = sumOfWeights * (sigma_xx * sigma_r2r2 - sigma_xr2 * sigma_xr2);
        double Sv = sumOfInvDeltaZ2;

        double sigma_rho_rho       = C * (4.*sigma_xx - 4.*rho*rho*(x2_av*x2_av - x_av*xr2_av + 0.25*rho*rho*xr2_av*xr2_av));
        double sigma_rho_phi       = C * (2.*sigma_xr2 - rho*rho*(2.*x2_av*xr2_av - r2_av*xr2_av - x_av*r4_av + 0.5*rho*rho*xr2_av));
        double sigma_phi_phi       = C * (sigma_r2r2 + rho*rho*r4_av*(y2_av - 0.25*rho*rho*r4_av));
        double sigma_rho_d         = C * (2.*x_av*xr2_av - 2.*x2_av*r2_av - rho*rho*(xr2_av*xr2_av - x2_av*r4_av));
        double sigma_phi_d         = C * (x_av*r4_av - r2_av*xr2_av);
        double sigma_d_d           = C * (x2_av*r4_av - xr2_av*xr2_av);

        double sigma_lambda_lambda;
        double sigma_lambda_z0;
        double sigma_z0_z0;
        if (zZeroPresent) {
            sigma_lambda_lambda = 0.;
            sigma_lambda_z0     = 0.;
            sigma_z0_z0         = 0.;
        } else {
            sigma_lambda_lambda = 1. / (Sv*simga_r_r * TMath::Power(1. + sigma_z_r/sigma_r_r*sigma_z_r/sigma_r_r), 2.);
            sigma_lambda_z0     = - r_av / (Sv*sigma_r_r * (1. + sigma_z_r/sigma_r_r*sigma_z_r/sigma_r_r));
            sigma_z0_z0         = r2_av / (Sv*sigma_r_r);
        }

        // Now populate the covariance matrix

        // upper left 3x3 block
        covarianceMatrix[0][0] = sigma_rho_rho;
        covarianceMatrix[0][1] = covarianceMatrix[1][0] = sigma_rho_phi;
        covarianceMatrix[0][2] = covarianceMatrix[2][0] = sigma_rho_d;
        covarianceMatrix[1][1] = sigma_phi_phi;
        covarianceMatrix[1][2] = covarianceMatrix[2][1] = sigma_phi_d;
        covarianceMatrix[2][2] = sigma_d_d;

        // lower right 2x2 block
        covarianceMatrix[3][3] = sigma_lambda_lambda;
        covarianceMatrix[3][4] = covarianceMatrix[4][3] = simga_lambda_z0;
        covarianceMatrix[4][4] = sigma_z0_z0;

        // all other covariance matrix elements zero
        // i.e. ignoring any correlations between (rho, phi, d) and (lambda, z0)

        trackParametrized = true;

    }

}

