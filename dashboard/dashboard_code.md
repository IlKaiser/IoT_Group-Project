# Dahsboard code

## Libraries used

- alert
- aws-sdk
- config
- crypto
- dotenv
- express
- mongodb
- mongoose

## Code organization

The dashboard code has been organized in several directories.

### Root directory

- **index.js**: it contains the code needed to launch the server
- **package.json**: it contains all dependencies for the server

### config

- **default.json**: it contains all configuration parameters

### helper

- **quickSort.js**: it contains the quicksort algorithm code
- **check_login.js**: it contains the main code to manage the login

### routes

- **views.js**: it contains all routes needed to manage the views on the frontend side
- **violations.js**: it contains all routes needed to manage the violations retrieved from the cloud on AWS

### startup

- **db.js**: it contains the code needed to correctly initialize the interaction with DynamoDB on AWS
- **routes.js**: it contains the code needed to use the routes available on the server

### views

- **login.html**: it contains the code for the login view
- **redirect_to_violations.html**: it contains the code for redirecting to the violations view
- **violations.html**: it contains the code for the violations view
