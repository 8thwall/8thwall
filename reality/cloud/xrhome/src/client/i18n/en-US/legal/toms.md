This document describes the technical and organizational security measures and controls implemented by 8th Wall Inc. to protect the data customers entrust to us as part of the 8th Wall's service offerings.
- “Developer” means a person with an 8th Wall account and is considered a Data Controller as per GDPR unless otherwise specified.
- “Developer Data” means any information provided or submitted by the Developer that is processed by 8th Wall.
- “End User” means a person who views an app or web page created by the Developer, which may or may not be hosted on the 8th Wall platform.
- “End User Data” means any information provided or submitted by the End User that is processed by 8th Wall.
- “Personal Data” means any information relating to an identified or identifiable natural person
- “Personnel” means 8th Wall employees, consultants and authorized subprocessors.
- “Strong Encryption” means the use of industry-standard encryption measures.
- “8th Wall” means 8th Wall Inc., headquartered in Palo Alto, California, USA.

## Organization Of Information Security

- 8th Wall employs full-time engineering Personnel responsible for maintaining information security.
- All Personnel responsible for information security report directly to the Director of Engineering or Chief Executive Officer.
- All Personnel have signed legally reviewed confidentiality agreements.
- All Personnel are given training for data privacy and information security upon hire.

## Physical Access

- The 8th Wall platform operates from several lead cloud providers that run inside certified third-party production data centers with a protected physical perimeter, strong physical controls, electronic access control, human security personnel, video surveillance, and electronic intrusion detection systems.
- Power and telecommunications cabling carrying Developer Data and End User Data or supporting information services at the production data centers are protected from interception, interference and damage.
- The production data centers and their equipment are physically protected against natural disasters, unauthorized entry, malicious attacks, and accidents.
- Equipment at the production data center is protected from power failures and other disruptions caused by failures in supporting utilities and is appropriately maintained.
- Physical access to the 8th Wall corporate location is controlled 24 hours a day by secured badge access, electronic intrusion detection systems and video surveillance.

## System Access

- Access to 8th Wall systems is granted only to Personnel and access is strictly limited as required for those persons to fulfil their function.
- 8th Wall has a password policy that prohibits the sharing of passwords and requires passwords to be changed on a regular basis. All passwords must fulfill defined minimum complexity requirements and are stored in encrypted form.
- Access to systems containing Developer Data and End User Data require two-factor authentication and/or account federation via open-standards (OAuth2, SAML 2.0, or similar) from a certified third-party identity service with two-factor authentication.
- All communication with systems containing Developer Data and End User Data requires the use of Strong Encryption via protocols such as HTTPS, SSL/TLS, and similar.
- Access to Developer Data and End User Data is terminated when Personnel leave the company.
- Personnel access to cloud services containing Developer Data and End User Data is logged and monitored.

## Data Access

- 8th Wall restricts Personnel access to Developer Data and End User Data on a “need-to-know” basis.
- Each such access and its subsequent operations are logged and monitored.
- Personnel training covers access rights to and general guidelines on definition and use of Developer Data and End User Data.

## Availability Controls

- Source code and configuration data is continually backed up to leading source control cloud provider.
- Databases and compiled code are subject to regular automated backups.

## Controls for separation of duties

- Multi-tiered environments separate development, staging, and production servers into isolated systems.
- Active development occurs on test databases in the development environment that are isolated from actual Developer Data and End User Data.
- The staging environment allows 8th Wall Personnel to test and catch errors in new versions of the software prior to their release to the production environment.
