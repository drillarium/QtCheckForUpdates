# QtCheckForUpdates

QtCheckForUpdates is a reusable and generic Qt-based `QDialog` implementation that provides functionality to check for updates in a Qt application. It fetches update information from a remote server, downloads the installer for the latest version, and allows the user to execute it to install the update.

## Features

- **Check for Updates**: Automatically fetches update information from a JSON endpoint.
- **Download Progress**: Displays download progress for large installers.
- **Installer Execution**: Executes the downloaded installer with optional elevated privileges.
- **Customizable**: Can be easily adapted to different update server endpoints.

## Requirements

- **Qt Version**: 5.12 or higher
- **C++ Standard**: C++11 or higher
- **Dependencies**:
  - QtCore
  - QtNetwork
  - QtWidgets

## How It Works

1. **Update Check**:
   - The dialog sends a GET request to a predefined URL to fetch update details (e.g., latest version and download link).
   - The server response must be a JSON object with at least the following fields:
     ```json
     {
       "version": "x.y.z",
       "url": "https://example.com/installer.exe"
     }
     ```

2. **Version Comparison**:
   - The current version of the application is compared with the latest version using `QVersionNumber`.

3. **Download and Execute**:
   - If an update is available, the user can download the installer.
   - A progress bar shows the download progress.
   - Once downloaded, the installer is executed.

## JSON Endpoint Example
The server must return a JSON response similar to the following:
```json
{
  "version": "1.2.3",
  "url": "https://example.com/myapp_installer.exe"
}
```

## Integration Steps

1. **Include the Dialog in Your Project**:
   - Add `UpdateCheckDialog.h` and `UpdateCheckDialog.cpp` to your project.

2. **Open the Dialog from Your Application**:
   - Create an instance of `UpdateCheckDialog` in your main application and open it in non-modal mode:
     ```cpp
     UpdateCheckDialog *dialog = new UpdateCheckDialog(this);
     dialog->setAttribute(Qt::WA_DeleteOnClose);
     dialog->show();
     ```

3. **Set the Update Server URL**:
   - Modify the `checkForUpdates()` method in `UpdateCheckDialog` to point to your update JSON endpoint:
     ```cpp
     QUrl updateUrl("https://yourserver.com/update.json");
     ```

4. **Version Management**:
   - Define the current version of your application using `QVersionNumber`:
     ```cpp
     QVersionNumber currentVersion = QVersionNumber::fromString("1.0.0");
     ```

5. **Build and Test**:
   - Build your application and test the update process.

## Screenshots

_**Coming soon!**_

## Future Enhancements

- **Proxy Support**: Add support for downloading through proxy servers.
- **Localization**: Provide translations for the dialog.
- **Elevated Installer Execution**: Automatically detect and request administrative privileges when executing the installer.

## License

This project is open source and distributed under the MIT License. See the LICENSE file for details.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests on the GitHub repository.
