//
//  ExellioDriver.swift
//  NativeAPI
//
//  Created by Artem Kirichek on 2/10/19.
//

import Foundation
import MiniPosLib

class MiniPosWrapper: NSObject, PBPinPadDelegate {
    
    @objc
    static let shared: MiniPosWrapper = MiniPosWrapper()
    
    @objc
    weak var delegate: MiniPosWrapperDelegate?
    
    fileprivate let pinPad: PBPinPadDevice
    
    override init() {
        pinPad = PBDeviceManager.shared.pinPad
        super.init()
    }
    
    @objc
    func connect() {
        pinPad.delegate = self
        pinPad.connect()
    }
    
    @objc
    func startTransaction(amout: Double, purpose: String, token: String) {
        pinPad.startTransaction(amount: amout, purpose: purpose, token: token)
    }
    
    @objc
    func reverseRequest(transactionId: String, serialNumber: String, token: String) {
        pinPad.reverseRequest(transactionId: transactionId,
                              serialNumber: serialNumber,
                              token: token) { [weak self] (result) in
                                switch result {
                                case .failure(let error):
                                    self?.delegate?.miniPosWrapperReverseRequestFailureWithErrorCode(error.code)
                                case .success:
                                    self?.delegate?.miniPosWrapperReverseRequestSuccess()
                                }
        }
    }
    
//    fileprivate func bind() {
//        pinPad.bindRequest(serialNumber: serialNumber,
//                           token: token) { [weak self] (result) in
//                            switch result {
//                            case .success:
//                                self?.showInfoMessage("Использование терминала для текущего пользователя РАЗРЕШЕНО")
//                            case .failure(let error):
//                                debugPrint("BIND REQUEST ERROR: \(error.localizedDescription)")
//                                self?.showInfoMessage("Использование терминала для текущего пользователя ЗАПРЕЩЕНО")
//                            }
//        }
//    }
 
    // MARK: - PBPinPadDelegate
    
    func didChangeConnectionState(_ state: DeviceConnectionState) {
        delegate?.miniPosWrapperDeviceConnectionState(state.rawValue)
        switch state {
        case .connected:
            pinPad.info { (info) in
                if let info = info {
                    DispatchQueue.main.async { [weak self] in
                        self?.connectedDeviceInfo(name: info.name ?? "", serialNumber: info.serialNumber ?? "", model: info.model ?? "")
                    }
                }
            }
        default:
            break
        }
    }
    
    func updateUserInterface(code: PinPadUserInterfaceCode, status: PinPadUserInterfaceStatus) {
        print(">> PinPad UI Code: \(code.code), \(code.message) >>")
        print(">> PinPad UI Status: \(status.code), \(status.message) >>")
    }
    
    func transactionFinished(result: TransactionResult) {
        switch result {
        case .failure(let error):
            var errorMessage: String = ""
            switch error {
            case .onlineResultFailed(let json):
                errorMessage = "Transaction online result failed with json: \(json)"
            case .reversed(let transaction):
                errorMessage = "Transaction reversed by id: \(transaction.json?["id"] as? String ?? "")"
            case .network(let error):
                errorMessage = "Network error code: \(error.code)"
                errorMessage += " Network error message: \(error.localizedDescription)"
                
            case .pinpad(let code, let message):
                errorMessage = "Transaction error code: \(code)"
                errorMessage += " Transaction error message: \(message)"
            default:
                errorMessage = "Transaction Finished with error code: \(error.code)"
                errorMessage = " Transaction Finished with error message: \(error.errorDescription ?? "")"
            }
            delegate?.miniPosWrapperTransactionFailureWithReason(errorMessage)
        case .success(let transaction):
            delegate?.miniPosWrapperTransactionSuccessWithJson(transaction.json ?? [:])
        }
    }
    
    func connectedDeviceInfo(name: String, serialNumber: String, model: String) {
        debugPrint(">> Name:  \(name)")
        debugPrint(">> SerialNumber: \(serialNumber)")
        debugPrint(">> Model: \(model)")
        // проверка привязки терминала к пользователю
        //bind()
        
        let deviceInfo = ["name": name, "serialNumber": serialNumber, "model": model]
        delegate?.miniPosWrapperConnectedDeviceInfo(deviceInfo)
    }
}

@objc
protocol MiniPosWrapperDelegate: NSObjectProtocol {
    
    func miniPosWrapperTransactionFailureWithReason(_ transactionFailureReason: String)
    
    func miniPosWrapperTransactionSuccessWithJson(_ json: Any)
    
    func miniPosWrapperReverseRequestSuccess()
    
    func miniPosWrapperReverseRequestFailureWithErrorCode(_ errorCode: Int)
    
    func miniPosWrapperDeviceConnectionState(_ state: Int32)
    
    func miniPosWrapperConnectedDeviceInfo(_ deviceInfo: Any)
}
