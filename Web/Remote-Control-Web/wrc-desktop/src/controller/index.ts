/**
 * WRC 服务的核心控制模块，分为 2 个部分
 * - client 处理 electron renderer process 的一些操作（采集/发布屏幕共享、接收远端的控制信令）
 * - backend 处理 electron main process 的一些操作（调用平台的原生模块执行远程控制）
 */

export * from "./client";
export * from "./backend";
